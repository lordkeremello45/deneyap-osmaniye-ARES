// ARES Go Bridge Service
// Copyright (C) 2026 ARES contributors
//
// This file is additionally offered under the GNU Affero General Public
// License v3.0 for the bridge_service/ component only. See ../LICENSE-AGPL-3.0
// and ../LICENSES.md. Other components retain their stated licensing terms.

package main

import (
	"context"
	"crypto/subtle"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"log"
	"math"
	"net"
	"net/http"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"
)

type Health struct {
	Status string `json:"status"`
}

type TelemetryResponse struct {
	Version int             `json:"version"`
	Source  string          `json:"source"`
	Status  string          `json:"status"`
	Data    json.RawMessage `json:"data"`
	Message string          `json:"message"`
}

type AudioRMS struct {
	Source       string    `json:"source"`
	CapturedAt   time.Time `json:"captured_at"`
	SampleRateHz int       `json:"sample_rate_hz"`
	Channels     int       `json:"channels"`
	RMS          float64   `json:"rms"`
	Peak         float64   `json:"peak"`
	ReceivedAt   time.Time `json:"received_at"`
}

var audioState struct {
	sync.RWMutex
	latest AudioRMS
	hasData bool
}

func bridgeAddress() (string, error) {
	addr := os.Getenv("ARES_BRIDGE_ADDR")
	if addr == "" {
		addr = "127.0.0.1:8080"
	}
	host, _, err := net.SplitHostPort(addr)
	if err != nil {
		return "", fmt.Errorf("invalid ARES_BRIDGE_ADDR %q: %w", addr, err)
	}
	ip := net.ParseIP(host)
	if ip == nil || !ip.IsLoopback() {
		return "", fmt.Errorf("ARES bridge may bind only to a numeric loopback IP, got %q", host)
	}
	return addr, nil
}

func writeJSON(w http.ResponseWriter, status int, value any) {
	w.Header().Set("Content-Type", "application/json; charset=utf-8")
	w.Header().Set("Cache-Control", "no-store")
	w.Header().Set("X-Content-Type-Options", "nosniff")
	w.WriteHeader(status)
	if err := json.NewEncoder(w).Encode(value); err != nil {
		log.Printf("encode response: %v", err)
	}
}

func bridgeToken() string { return os.Getenv("ARES_BRIDGE_TOKEN") }

func tokenConfigured() bool {
	return len(bridgeToken()) >= 32
}

func requireBridgeToken(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		expected := bridgeToken()
		provided := r.Header.Get("Authorization")
		const prefix = "Bearer "
		if len(expected) < 32 || len(provided) < len(prefix) ||
			provided[:len(prefix)] != prefix ||
			subtle.ConstantTimeCompare([]byte(provided[len(prefix):]), []byte(expected)) != 1 {
			writeJSON(w, http.StatusUnauthorized, map[string]string{"status": "unauthorized"})
			return
		}
		next(w, r)
	}
}

func receiveAudioRMS(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		w.Header().Set("Allow", http.MethodPost)
		writeJSON(w, http.StatusMethodNotAllowed, map[string]string{"status": "method_not_allowed"})
		return
	}
	if r.Header.Get("Content-Type") != "application/json" {
		writeJSON(w, http.StatusUnsupportedMediaType, map[string]string{"status": "content_type_must_be_application_json"})
		return
	}
	r.Body = http.MaxBytesReader(w, r.Body, 4096)
	dec := json.NewDecoder(r.Body)
	dec.DisallowUnknownFields()
	var sample AudioRMS
	if err := dec.Decode(&sample); err != nil {
		writeJSON(w, http.StatusBadRequest, map[string]string{"status": "invalid_json"})
		return
	}
	var extra any
	if err := dec.Decode(&extra); !errors.Is(err, io.EOF) {
		writeJSON(w, http.StatusBadRequest, map[string]string{"status": "invalid_json"})
		return
	}

	now := time.Now().UTC()
	if sample.Source != "xvf3800_uac2" ||
		sample.SampleRateHz < 8000 || sample.SampleRateHz > 96000 ||
		sample.Channels < 1 || sample.Channels > 8 ||
		math.IsNaN(sample.RMS) || math.IsInf(sample.RMS, 0) ||
		math.IsNaN(sample.Peak) || math.IsInf(sample.Peak, 0) ||
		sample.RMS < 0 || sample.RMS > 1 ||
		sample.Peak < 0 || sample.Peak > 1 ||
		sample.RMS > sample.Peak+0.02 ||
		sample.CapturedAt.IsZero() ||
		sample.CapturedAt.After(now.Add(2*time.Second)) ||
		now.Sub(sample.CapturedAt) > 5*time.Second {
		writeJSON(w, http.StatusUnprocessableEntity, map[string]string{"status": "invalid_audio_sample"})
		return
	}

	sample.CapturedAt = sample.CapturedAt.UTC()
	sample.ReceivedAt = now
	audioState.Lock()
	audioState.latest = sample
	audioState.hasData = true
	audioState.Unlock()
	writeJSON(w, http.StatusAccepted, map[string]string{"status": "accepted"})
}

func readAudioRMS(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		w.Header().Set("Allow", http.MethodGet)
		writeJSON(w, http.StatusMethodNotAllowed, map[string]string{"status": "method_not_allowed"})
		return
	}
	audioState.RLock()
	sample, ok := audioState.latest, audioState.hasData
	audioState.RUnlock()
	if !ok || time.Since(sample.ReceivedAt) > 3*time.Second {
		writeJSON(w, http.StatusServiceUnavailable, map[string]any{
			"status": "waiting_for_xvf3800",
			"data":   nil,
		})
		return
	}
	writeJSON(w, http.StatusOK, map[string]any{
		"status": "ok",
		"data": map[string]any{
			"source": sample.Source,
			"captured_at": sample.CapturedAt,
			"sample_rate_hz": sample.SampleRateHz,
			"channels": sample.Channels,
			"rms": sample.RMS,
			"peak": sample.Peak,
			"age_ms": time.Since(sample.ReceivedAt).Milliseconds(),
		},
	})
}

func newHandler() http.Handler {
	mux := http.NewServeMux()
	mux.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			w.Header().Set("Allow", http.MethodGet)
			writeJSON(w, http.StatusMethodNotAllowed, map[string]string{"status": "method_not_allowed"})
			return
		}
		writeJSON(w, http.StatusOK, Health{Status: "ok"})
	})
	mux.HandleFunc("/api/v1/telemetry", requireBridgeToken(func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			w.Header().Set("Allow", http.MethodGet)
			writeJSON(w, http.StatusMethodNotAllowed, map[string]string{"status": "method_not_allowed"})
			return
		}
		// The full serial telemetry adapter is not implemented yet. Audio RMS
		// is exposed separately and must not be mistaken for complete telemetry.
		writeJSON(w, http.StatusServiceUnavailable, TelemetryResponse{
			Version: 1, Source: "ares", Status: "waiting_for_device",
			Data: json.RawMessage("null"),
			Message: "Full telemetry source is not connected; values are unavailable.",
		})
	}))
	mux.HandleFunc("/api/v1/audio/rms", requireBridgeToken(receiveAudioRMS))
	mux.HandleFunc("/api/v1/audio", requireBridgeToken(readAudioRMS))
	return mux
}

func main() {
	if !tokenConfigured() {
		log.Fatal("ARES_BRIDGE_TOKEN must be set to a randomly generated secret of at least 32 characters")
	}
	addr, err := bridgeAddress()
	if err != nil {
		log.Fatal(err)
	}

	server := &http.Server{
		Addr: addr, Handler: newHandler(),
		ReadHeaderTimeout: 5 * time.Second,
		ReadTimeout: 10 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout: 60 * time.Second,
	}

	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()
	go func() {
		<-ctx.Done()
		shutdownCtx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()
		if err := server.Shutdown(shutdownCtx); err != nil {
			log.Printf("HTTP shutdown: %v", err)
		}
	}()

	log.Printf("ARES Go Bridge listening on %s", addr)
	if err := server.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
		log.Fatalf("HTTP server: %v", err)
	}
}
