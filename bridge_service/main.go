// ARES Go Bridge Service
// Copyright (C) 2026 ARES contributors
//
// This file is additionally offered under the GNU Affero General Public
// License v3.0 for the bridge_service/ component only. See ../LICENSE-AGPL-3.0
// and ../LICENSES.md. Other components retain their stated licensing terms.

package main

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"os/signal"
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

func bridgeAddress() (string, error) {
	addr := os.Getenv("ARES_BRIDGE_ADDR")
	if addr == "" {
		addr = "127.0.0.1:8080"
	}
	host, _, err := net.SplitHostPort(addr)
	if err != nil {
		return "", fmt.Errorf("invalid ARES_BRIDGE_ADDR %q: %w", addr, err)
	}
	if host != "localhost" {
		ip := net.ParseIP(host)
		if ip == nil || !ip.IsLoopback() {
			return "", fmt.Errorf("ARES bridge is an unauthenticated prototype and may bind only to loopback, got %q", host)
		}
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
	mux.HandleFunc("/api/v1/telemetry", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			w.Header().Set("Allow", http.MethodGet)
			writeJSON(w, http.StatusMethodNotAllowed, map[string]string{"status": "method_not_allowed"})
			return
		}
		// No serial/MQTT device adapter exists yet. Never fabricate live sensor data
		// or return HTTP 200 for a telemetry endpoint that has no source.
		writeJSON(w, http.StatusServiceUnavailable, TelemetryResponse{
			Version: 1,
			Source:  "ares",
			Status:  "waiting_for_device",
			Data:    json.RawMessage("null"),
			Message: "No telemetry source is connected; values are unavailable.",
		})
	})
	return mux
}

func main() {
	addr, err := bridgeAddress()
	if err != nil {
		log.Fatal(err)
	}

	server := &http.Server{
		Addr:              addr,
		Handler:           newHandler(),
		ReadHeaderTimeout: 5 * time.Second,
		ReadTimeout:       10 * time.Second,
		WriteTimeout:      10 * time.Second,
		IdleTimeout:       60 * time.Second,
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
