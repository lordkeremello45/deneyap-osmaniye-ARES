#pragma once

// Compile-only CI configuration. These are deliberately fake values.
// This header must never be used for deployment or flashed onto a real node.
#define ARES_MQTT_ENABLED
#define ARES_WIFI_SSID "ARES_CI_ONLY"
#define ARES_WIFI_PASSWORD "not-a-real-password"
#define ARES_MQTT_HOST "mqtt-ci.invalid"
#define ARES_MQTT_USERNAME "ci-only-user"
#define ARES_MQTT_PASSWORD "ci-only-secret"
#define ARES_MQTT_CLIENT_ID "ares-ci-only"
#define ARES_MQTT_ROOT_CA "-----BEGIN CERTIFICATE-----\nCI_ONLY_NOT_A_REAL_CERTIFICATE\n-----END CERTIFICATE-----\n"
