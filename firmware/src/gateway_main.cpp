#include <Arduino.h>
#include <cstring>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#if __has_include("ares_board_config.h")
#include "ares_board_config.h"
#endif
#if __has_include("ares_secrets.h")
#include "ares_secrets.h"
#endif

namespace {
constexpr uint32_t kLinkBaud = 115200;
constexpr size_t kMaxFrame = 1024;
constexpr uint32_t kReconnectIntervalMs = 5000;
constexpr char kTelemetryTopic[] = "ares/v1/telemetry/card2";

#if defined(ARES_LINK_RX_PIN) && defined(ARES_LINK_TX_PIN)
constexpr bool kLinkConfigured = true;
#else
constexpr bool kLinkConfigured = false;
#endif

#if defined(ARES_MQTT_ENABLED)
#ifndef ARES_WIFI_SSID
#error "ARES_MQTT_ENABLED requires ARES_WIFI_SSID in untracked ares_secrets.h"
#endif
#ifndef ARES_WIFI_PASSWORD
#error "ARES_MQTT_ENABLED requires ARES_WIFI_PASSWORD in untracked ares_secrets.h"
#endif
#ifndef ARES_MQTT_HOST
#error "ARES_MQTT_ENABLED requires ARES_MQTT_HOST in untracked ares_secrets.h"
#endif
#ifndef ARES_MQTT_USERNAME
#error "ARES_MQTT_ENABLED requires a unique ARES_MQTT_USERNAME"
#endif
#ifndef ARES_MQTT_PASSWORD
#error "ARES_MQTT_ENABLED requires a unique ARES_MQTT_PASSWORD"
#endif
#ifndef ARES_MQTT_ROOT_CA
#error "ARES_MQTT_ENABLED requires ARES_MQTT_ROOT_CA; TLS verification must not be disabled"
#endif
#ifndef ARES_MQTT_CLIENT_ID
#error "ARES_MQTT_ENABLED requires a unique ARES_MQTT_CLIENT_ID"
#endif
WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);
uint32_t lastReconnectAttempt = 0;
#endif

char frame[kMaxFrame + 1];
size_t frameLength = 0;
bool discardUntilNewline = false;

bool validTelemetryFrame(const char* payload) {
  JsonDocument document;
  const DeserializationError error = deserializeJson(document, payload);
  if (error || !document["type"].is<const char*>() ||
      strcmp(document["type"].as<const char*>(), "sensor_status") != 0 ||
      document["version"].as<int>() != 1 ||
      !document["timestamp_ms"].is<uint32_t>()) {
    return false;
  }
  return true;
}

#if defined(ARES_MQTT_ENABLED)
void maintainMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqttClient.connected()) return;
  const uint32_t now = millis();
  if (uint32_t(now - lastReconnectAttempt) < kReconnectIntervalMs) return;
  lastReconnectAttempt = now;
  if (!mqttClient.connect(ARES_MQTT_CLIENT_ID, ARES_MQTT_USERNAME, ARES_MQTT_PASSWORD)) {
    Serial.printf("MQTT connect failed, state=%d\n", mqttClient.state());
  } else {
    Serial.println("MQTT TLS connection established; telemetry-only client.");
  }
}
#endif

void processFrame() {
  if (discardUntilNewline || frameLength == 0) {
    frameLength = 0;
    discardUntilNewline = false;
    return;
  }
  frame[frameLength] = '\0';
  if (!validTelemetryFrame(frame)) {
    Serial.println("Rejected malformed or unexpected sensor telemetry frame.");
    frameLength = 0;
    return;
  }

#if defined(ARES_MQTT_ENABLED)
  if (mqttClient.connected()) {
    if (!mqttClient.publish(kTelemetryTopic, frame, false)) {
      Serial.println("MQTT publish failed; frame dropped, not retained.");
    }
  } else {
    Serial.println("MQTT unavailable; current frame dropped (no stale replay).");
  }
#else
  // Safe local bring-up path before broker/cert/credentials are configured.
  Serial.println(frame);
#endif
  frameLength = 0;
}
}

void setup() {
  Serial.begin(115200);
  delay(50);
  if (!kLinkConfigured) {
    Serial.println("ARES gateway: inter-board UART disabled; configure verified raw GPIOs in ares_board_config.h.");
    return;
  }

#if defined(ARES_LINK_RX_PIN) && defined(ARES_LINK_TX_PIN)
  Serial2.begin(kLinkBaud, SERIAL_8N1, ARES_LINK_RX_PIN, ARES_LINK_TX_PIN);
#endif

#if defined(ARES_MQTT_ENABLED)
  tlsClient.setCACert(ARES_MQTT_ROOT_CA);
  mqttClient.setServer(ARES_MQTT_HOST, 8883);
  mqttClient.setBufferSize(kMaxFrame + 128);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ARES_WIFI_SSID, ARES_WIFI_PASSWORD);
  Serial.println("ARES gateway: Wi-Fi/MQTT TLS enabled; publishing telemetry only.");
#else
  Serial.println("ARES gateway: MQTT disabled until a local ares_secrets.h configures TLS and per-device credentials.");
#endif
}

void loop() {
  if (!kLinkConfigured) {
    delay(100);
    return;
  }

#if defined(ARES_MQTT_ENABLED)
  maintainMqtt();
  mqttClient.loop();
#endif

  while (Serial2.available() > 0) {
    const char c = static_cast<char>(Serial2.read());
    if (c == '\n') {
      processFrame();
      continue;
    }
    if (c == '\r') continue;
    if (discardUntilNewline) continue;
    if (frameLength >= kMaxFrame) {
      discardUntilNewline = true;
      frameLength = 0;
      Serial.println("Rejected overlong UART frame.");
      continue;
    }
    // JSON frames are line-delimited. Control bytes are not accepted.
    if (static_cast<unsigned char>(c) < 0x20U) {
      discardUntilNewline = true;
      frameLength = 0;
      continue;
    }
    frame[frameLength++] = c;
  }
  delay(1);
}
