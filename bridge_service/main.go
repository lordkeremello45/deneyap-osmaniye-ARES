// ARES Go Bridge Service
// Copyright (C) 2026 ARES contributors
//
// This file is additionally offered under the GNU Affero General Public
// License v3.0 for the bridge_service/ component only. See ../LICENSE-AGPL-3.0
// and ../LICENSES.md. The repository's other components retain their
// separately stated licensing terms.

package main

import (
	"encoding/json"
	"log"
	"net/http"
)

type Health struct {
	Status string `json:"status"`
}

func main() {
	mux := http.NewServeMux()
	mux.HandleFunc("/health", func(w http.ResponseWriter, _ *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		_ = json.NewEncoder(w).Encode(Health{Status: "ok"})
	})
	mux.HandleFunc("/api/v1/telemetry", func(w http.ResponseWriter, _ *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		_, _ = w.Write([]byte(`{"version":1,"source":"ares","status":"waiting_for_device"}`))
	})
	log.Println("ARES Go Bridge listening on :8080")
	log.Fatal(http.ListenAndServe(":8080", mux))
}
