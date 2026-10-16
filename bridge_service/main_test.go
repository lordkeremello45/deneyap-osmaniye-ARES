package main

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"
	"time"
)

func TestHealthReturnsOK(t *testing.T) {
	req := httptest.NewRequest(http.MethodGet, "/health", nil)
	rec := httptest.NewRecorder()
	newHandler().ServeHTTP(rec, req)
	if rec.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", rec.Code, http.StatusOK)
	}
	var got Health
	if err := json.Unmarshal(rec.Body.Bytes(), &got); err != nil {
		t.Fatalf("invalid JSON: %v", err)
	}
	if got.Status != "ok" {
		t.Fatalf("status = %q, want ok", got.Status)
	}
	if rec.Header().Get("Cache-Control") != "no-store" {
		t.Fatal("health response must not be cached")
	}
}

func TestTelemetryDoesNotPretendDeviceIsConnected(t *testing.T) {
	req := httptest.NewRequest(http.MethodGet, "/api/v1/telemetry", nil)
	rec := httptest.NewRecorder()
	newHandler().ServeHTTP(rec, req)
	if rec.Code != http.StatusServiceUnavailable {
		t.Fatalf("status = %d, want %d", rec.Code, http.StatusServiceUnavailable)
	}
	var got TelemetryResponse
	if err := json.Unmarshal(rec.Body.Bytes(), &got); err != nil {
		t.Fatalf("invalid JSON: %v", err)
	}
	if got.Status != "waiting_for_device" || string(got.Data) != "null" {
		t.Fatalf("unexpected response: %+v", got)
	}
}

func TestTelemetryRejectsNonGetMethod(t *testing.T) {
	req := httptest.NewRequest(http.MethodPost, "/api/v1/telemetry", nil)
	rec := httptest.NewRecorder()
	newHandler().ServeHTTP(rec, req)
	if rec.Code != http.StatusMethodNotAllowed {
		t.Fatalf("status = %d, want %d", rec.Code, http.StatusMethodNotAllowed)
	}
	if rec.Header().Get("Allow") != http.MethodGet {
		t.Fatalf("Allow = %q, want GET", rec.Header().Get("Allow"))
	}
}

func TestBridgeAddressIsLoopbackOnly(t *testing.T) {
	t.Setenv("ARES_BRIDGE_ADDR", "127.0.0.1:8080")
	if got, err := bridgeAddress(); err != nil || got != "127.0.0.1:8080" {
		t.Fatalf("loopback address rejected: got %q, err %v", got, err)
	}

	t.Setenv("ARES_BRIDGE_ADDR", "0.0.0.0:8080")
	if _, err := bridgeAddress(); err == nil {
		t.Fatal("expected non-loopback bind to be rejected")
	}
}


func TestAudioRMSRequiresFreshXVF3800Sample(t *testing.T) {
	audioState.Lock()
	audioState.latest = AudioRMS{}
	audioState.hasData = false
	audioState.Unlock()

	get := httptest.NewRecorder()
	newHandler().ServeHTTP(get, httptest.NewRequest(http.MethodGet, "/api/v1/audio", nil))
	if get.Code != http.StatusServiceUnavailable {
		t.Fatalf("expected unavailable before sample, got %d", get.Code)
	}

	sample := map[string]any{
		"source": "xvf3800_uac2",
		"captured_at": time.Now().UTC().Format(time.RFC3339Nano),
		"sample_rate_hz": 16000,
		"channels": 2,
		"rms": 0.12,
		"peak": 0.31,
	}
	body, err := json.Marshal(sample)
	if err != nil {
		t.Fatal(err)
	}
	post := httptest.NewRecorder()
	newHandler().ServeHTTP(post, httptest.NewRequest(http.MethodPost, "/api/v1/audio/rms", bytes.NewReader(body)))
	if post.Code != http.StatusAccepted {
		t.Fatalf("expected accepted sample, got %d: %s", post.Code, post.Body.String())
	}

	get = httptest.NewRecorder()
	newHandler().ServeHTTP(get, httptest.NewRequest(http.MethodGet, "/api/v1/audio", nil))
	if get.Code != http.StatusOK {
		t.Fatalf("expected fresh sample, got %d: %s", get.Code, get.Body.String())
	}
}

func TestAudioRMSRejectsInvalidSamples(t *testing.T) {
	sample := map[string]any{
		"source": "xvf3800_uac2",
		"captured_at": time.Now().UTC().Format(time.RFC3339Nano),
		"sample_rate_hz": 16000,
		"channels": 2,
		"rms": 0.8,
		"peak": 0.2,
	}
	body, err := json.Marshal(sample)
	if err != nil {
		t.Fatal(err)
	}
	response := httptest.NewRecorder()
	newHandler().ServeHTTP(response, httptest.NewRequest(http.MethodPost, "/api/v1/audio/rms", bytes.NewReader(body)))
	if response.Code != http.StatusUnprocessableEntity {
		t.Fatalf("expected invalid sample rejection, got %d", response.Code)
	}
}


func TestStatusDistinguishesUnavailableTelemetryAndAudio(t *testing.T) {
	audioState.Lock()
	audioState.latest = AudioRMS{}
	audioState.hasData = false
	audioState.Unlock()

	response := httptest.NewRecorder()
	newHandler().ServeHTTP(response, httptest.NewRequest(http.MethodGet, "/api/v1/status", nil))
	if response.Code != http.StatusOK {
		t.Fatalf("status = %d, want %d", response.Code, http.StatusOK)
	}

	var got map[string]any
	if err := json.Unmarshal(response.Body.Bytes(), &got); err != nil {
		t.Fatalf("invalid JSON: %v", err)
	}
	if got["service"] != "ok" {
		t.Fatalf("service = %v, want ok", got["service"])
	}
	telemetry, ok := got["telemetry"].(map[string]any)
	if !ok || telemetry["available"] != false || telemetry["status"] != "waiting_for_device" {
		t.Fatalf("telemetry must explicitly report unavailable: %v", got["telemetry"])
	}
	audio, ok := got["audio"].(map[string]any)
	if !ok || audio["available"] != false || audio["status"] != "waiting_for_xvf3800" {
		t.Fatalf("audio must explicitly report unavailable: %v", got["audio"])
	}
}

func TestAudioRMSReportsStaleDataSeparately(t *testing.T) {
	audioState.Lock()
	audioState.latest = AudioRMS{Source: "xvf3800_uac2", ReceivedAt: time.Now().Add(-5 * time.Second)}
	audioState.hasData = true
	audioState.Unlock()

	response := httptest.NewRecorder()
	newHandler().ServeHTTP(response, httptest.NewRequest(http.MethodGet, "/api/v1/audio", nil))
	if response.Code != http.StatusServiceUnavailable {
		t.Fatalf("status = %d, want %d", response.Code, http.StatusServiceUnavailable)
	}
	var got map[string]any
	if err := json.Unmarshal(response.Body.Bytes(), &got); err != nil {
		t.Fatalf("invalid JSON: %v", err)
	}
	if got["status"] != "stale" || got["data"] != nil {
		t.Fatalf("stale sample must not be returned as current data: %v", got)
	}
}

func TestAudioRMSRejectsUnknownFields(t *testing.T) {
	sample := map[string]any{
		"source": "xvf3800_uac2",
		"captured_at": time.Now().UTC().Format(time.RFC3339Nano),
		"sample_rate_hz": 16000,
		"channels": 2,
		"rms": 0.12,
		"peak": 0.31,
		"unexpected": true,
	}
	body, err := json.Marshal(sample)
	if err != nil {
		t.Fatal(err)
	}
	response := httptest.NewRecorder()
	newHandler().ServeHTTP(response, httptest.NewRequest(http.MethodPost, "/api/v1/audio/rms", bytes.NewReader(body)))
	if response.Code != http.StatusBadRequest {
		t.Fatalf("status = %d, want %d", response.Code, http.StatusBadRequest)
	}
}

func TestStatusRejectsNonGetMethod(t *testing.T) {
	response := httptest.NewRecorder()
	newHandler().ServeHTTP(response, httptest.NewRequest(http.MethodPost, "/api/v1/status", nil))
	if response.Code != http.StatusMethodNotAllowed {
		t.Fatalf("status = %d, want %d", response.Code, http.StatusMethodNotAllowed)
	}
	if response.Header().Get("Allow") != http.MethodGet {
		t.Fatalf("Allow = %q, want GET", response.Header().Get("Allow"))
	}
}
