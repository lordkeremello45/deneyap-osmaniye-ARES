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
Validation → Filtering → Sensor Fusion → Gemma 4 E2B Q5_K_M
      │ structured result
      ▼
Go Bridge → Flutter UI

Deneyap Kart V2 / ESP32 firmware
      │ UART + CRC16
      ├── Thermal / FLIR Lepton 3.5
      ├── UWB / Qorvo DWM3000
      ├── Acoustic / XMOS XVF3800
      ├── Seismic / Geospace GS-One LF + analog front-end
      └── LiDAR / Garmin LIDAR-Lite v3
```

## Proje sitesi

🌐 [ARES Web Sitesi](https://lordkeremello45.github.io/deneyap-osmaniye-ARES/)

`site/` yalnızca GitHub Pages proje tanıtım sayfasıdır; ana ürün Android/iOS Flutter uygulamasıdır.

## Repository

- `mobile_app/` — Flutter/Dart Android + iOS uygulaması
- `bridge_service/` — Go HTTP/WebSocket servis katmanı
- `ai_core/` — C++/CMake sensör füzyonu ve AI katmanı
- `firmware/` — Deneyap Kart V2 / ESP32 firmware
- `models/` — Gemma model metadata/configuration; model ağırlıkları Git'e eklenmez
- `docs/` — mimari, protokol, donanım, Ada/SPARK ve test dokümanları
- `site/` — proje web sitesi

## AI ve model

Seçilen model Gemma 4 E2B Instruct Q5_K_M GGUF'tur. Yaklaşık 3,66 GB'lık model dosyası ana bilgisayarda `llama.cpp` ile çalıştırılmak üzere `models/` altına yerleştirilir; Deneyap Kart V2 üzerinde çalıştırılmak üzere tasarlanmamıştır. Model dosyası henüz depoya eklenmemiştir ve gerçek inference adapter'ı tamamlanmış kabul edilmez.

AI Core ham yüksek frekanslı sensör akışını doğrudan modele vermez. Önce deterministik doğrulama, filtreleme ve sensör füzyonu yapılır. Model sonucu yalnızca operatör destek verisidir; uçuş kontrolü, motor PWM ve failsafe kararları modele bırakılmaz.

## Ada/SPARK

`docs/ada-spark/` içinde sensör girdisi doğrulama ve ihtiyatlı görev durumu seçimi için ayrı bir Ada/SPARK doğrulama başlangıcı bulunur. Bu, mevcut C++/Go/Flutter/ESP32 mimarisinin yerine geçmez. GNATprove sonucu alınana kadar biçimsel doğrulama başarılı kabul edilmez.

## Lisanslar

- Ana proje kaynak kodu: [GPL-3.0](LICENSE)
- Yalnızca `site/` web sitesi ve `bridge_service/` web/API hizmeti kaynakları için ek lisans: [AGPL-3.0](LICENSE-AGPL-3.0)
- Bileşen kapsamları: [LICENSES.md](LICENSES.md)
- Gemma model ağırlıkları ARES'in kaynak kodu lisanslarından bağımsız olarak Google Gemma şartlarına tabidir.

## Geliştirme

### Bridge

```sh
cd bridge_service
go run .
```

Health endpoint: `http://127.0.0.1:8080/health`

### C++ Core

```sh
cmake -S ai_core -B ai_core/build
cmake --build ai_core/build
```

### Firmware

PlatformIO ile `firmware/` hedefi derlenir. Kütüphane sürümleri ve donanım sınırlamaları için [sensör kütüphaneleri](docs/hardware/sensor-libraries.md) belgesine bakın. Kesin pinout, breakout kartları ve elektriksel arayüzler fiziksel olarak doğrulanmadan sabitlenmez.

## Dokümantasyon

- [Mimari](docs/architecture.md)
- [Donanım envanteri](docs/hardware/bom.md)
- [Sensör kütüphaneleri](docs/hardware/sensor-libraries.md)
- [UART protokolü](docs/protocol/uart-peer.md)
- [Ada/SPARK güvenlik iş akışı](docs/ada-spark/README.md)
- [Test planı](docs/validation/test-plan.md)

## Durum

ARES prototiptir. Sensör sonuçları operatör destek verisidir ve gerçek arama/kurtarma operasyonlarında sertifikalı sistemlerin yerine geçmez.
