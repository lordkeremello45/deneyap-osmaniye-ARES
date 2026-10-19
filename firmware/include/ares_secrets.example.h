#pragma once

// Copy to firmware/include/ares_secrets.h (the real file is git-ignored).
// Never commit the real file or credentials. Use a unique MQTT identity for
// this board. Do not enable MQTT until the broker has TLS, certificate
// validation, anonymous access disabled, and a least-privilege topic ACL.
//
// Uncomment after replacing every placeholder with real configuration:
// #define ARES_MQTT_ENABLED
// #define ARES_WIFI_SSID "REPLACE_WITH_WIFI_SSID"
// #define ARES_WIFI_PASSWORD "REPLACE_WITH_WIFI_PASSWORD"
// #define ARES_MQTT_HOST "broker.example.invalid"
// #define ARES_MQTT_USERNAME "unique_device_username"
// #define ARES_MQTT_PASSWORD "unique_random_device_secret"
// #define ARES_MQTT_CLIENT_ID "ares-card1-unique-id"
// #define ARES_MQTT_ROOT_CA \
//   "-----BEGIN CERTIFICATE-----\n" \
//   "REPLACE_WITH_REVIEWED_CA_BODY\n" \
//   "-----END CERTIFICATE-----\n"
