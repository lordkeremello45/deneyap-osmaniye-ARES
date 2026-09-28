# ARES — Deneyap Osmaniye

Drone destekli enkaz arama/kurtarma araştırma ve prototip sistemi.

## Gerçek yazılım mimarisi

```text
Android / iOS
Flutter + Dart
      │ HTTP / WebSocket
      ▼
Go Bridge Service
      │ local IPC
      ├──────────────► Raw telemetry / logging
      ▼
C++ AI Core
Validation → Filtering → Sensor Fusion → Gemma 3 1B Q5_K_M
      │ structured result
      ▼
Go Bridge → Flutter UI

Deneyap Kart V2 / ESP32 firmware
      │ UART + CRC16
      ├── Thermal / MLX90640-class
      ├── UWB / DW3000-class
      ├── Acoustic / INMP441-class I2S
      ├── Seismic / geophone + AFE
      └── LiDAR / VL53L1X-class
```

## Proje sitesi

🌐 [ARES Web Sitesi](https://lordkeremello45.github.io/deneyap-osmaniye-ARES/)

**Web sitesi bu sistemin parçası değildir.** `site/` yalnızca GitHub Pages proje tanıtım sayfasıdır. Asıl ürün Android/iOS Flutter uygulamasıdır.

## Repository

- `mobile_app/` — Flutter/Dart Android + iOS uygulaması
- `bridge_service/` — Go HTTP/WebSocket servis katmanı
- `ai_core/` — C++/CMake sensör füzyonu ve AI katmanı
- `firmware/` — Deneyap Kart V2 / ESP32 firmware
- `models/` — yerel Gemma model metadata/configuration
- `docs/` — mimari, protokol, donanım ve test dokümanları
- `site/` — yalnızca proje web sitesi

## Sensör yazılımı

Başlangıç PlatformIO bağımlılıkları `firmware/platformio.ini` içinde tutulur:

- Adafruit MLX90640 — termal sensör sınıfı
- SparkFun VL53L1X — LiDAR/ToF sınıfı
- ESP32 I2S — INMP441 sınıfı akustik giriş için yerleşik çevrebirim
- ADC + harici düşük gürültülü AFE — geofon/sismik giriş
- DW3000 sınıfı UWB — kesin modül seçimi sonrası doğrulanmış sürücü

Kesin sensör parçası BOM doğrulamasından sonra kilitlenir; pin ve elektriksel değerler tahmin edilmez.

## AI sınırı

AI Core ham yüksek frekanslı sensör akışını doğrudan modele vermez. Önce deterministik doğrulama, filtreleme ve sensör füzyonu yapılır. Gemma yapılandırılmış özellikleri yorumlar ve Go Bridge'e makine tarafından işlenebilir sonuç döndürür.

AI Core uçuş kontrolcüsü değildir; motor PWM, stabilizasyon ve failsafe kararları AI modeline bırakılmaz.

## Geliştirme

### Mobile

`mobile_app/` içinde Flutter kullanılır ve hedef platformlar yalnızca Android/iOS'tur. Flutter SDK ile platform projeleri oluşturulduktan sonra `flutter pub get` ve `flutter run` kullanılabilir.

### Bridge

```bash
cd bridge_service
go run .
```

Health: `http://127.0.0.1:8080/health`

### C++ Core

```bash
cmake -S ai_core -B ai_core/build
cmake --build ai_core/build
```

Gemma/llama.cpp gerçek inference adapter'ı model dosyası ve native runtime doğrulamasından sonra bağlanır; binary model dosyaları repoya gömülmez.

### Firmware

PlatformIO ile `firmware/` hedefi derlenir. Kesin pinout, sensör modülleri fiziksel olarak doğrulanmadan firmware'e sabitlenmez.

## Dokümantasyon

- [Mimari](docs/architecture.md)
- [Donanım envanteri](docs/hardware/bom.md)
- [UART protokolü](docs/protocol/uart-peer.md)
- [Test planı](docs/validation/test-plan.md)

## Durum

ARES prototiptir. Sensör sonuçları operatör destek verisidir ve gerçek arama/kurtarma operasyonlarında sertifikalı sistemlerin yerine geçmez.
