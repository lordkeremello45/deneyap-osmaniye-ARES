# ARES Yazılım ve Sistem Mimarisi

Repository görev bazlıdır. HWcontrol2.0'dan alınan temel prensipler; servis ayrımı, telemetry doğrulama, health/fail-safe, authenticated IPC ve deterministik AI ön işleme olarak uygulanır.

## Ana omurga

DRONE -> drone_telemetry -> validation -> raw storage + AI Core -> Go Bridge -> Android Controller

## Katmanlar

### drone_telemetry
Sistemin sürekli veri giriş katmanıdır. Sensör verisini alır, paketler, CRC/sequence/timestamp/range doğrulaması yapar, raw kayıt için ayırır ve doğrulanmış feature'ları AI Core'a aktarır.

### drone
Uçuş ve güç sınırıdır. Flight controller, motor/ESC telemetry, güç telemetry, safety ve failsafe burada kalır. AI Core uçuş komutu sahibi değildir.

### ai_core
C++20/CMake analiz katmanıdır:

validated telemetry -> filtering -> time window -> sensor fusion -> feature extraction -> risk/detection -> Gemma -> structured result

### bridge
Go servis katmanıdır. Telemetry routing, C++ IPC, Android API/WebSocket, HMAC authentication, nonce/replay protection, health, diagnostics, backpressure ve logging burada bulunur.

### controller/android
Flutter/Dart tabanlı Android operatör uygulamasıdır. Canlı telemetry, termal görünüm, harita, aday hedefler, health, uyarılar ve mission state gösterilir.

### storage
Raw telemetry ve mission loglarını AI sonuçlarından ayrı tutar.

### website/astro
Resmi ARES web sitesidir ve drone çalışma zamanından bağımsızdır.

## Telemetry mesaj sözleşmesi

{
  "version": 1,
  "sequence": 18452,
  "timestamp_ms": 1780000123456,
  "thermal": {"anomaly_c": 3.2, "hot_pixel_count": 14},
  "uwb_distance_m": 4.2,
  "acoustic": {"rms": 0.031, "peak": 0.18},
  "seismic": {"rms": 0.021, "peak": 0.09},
  "lidar_distance_m": 8.0
}

Bu paket AI'a doğrudan verilmez. Validator kabulünden sonra feature pipeline'a girer.

## Güvenlik

Hassas IPC/komutlar: HMAC-SHA-256, timestamp window, nonce, replay cache, request size/rate limits ve fail-closed hardware control.

Telemetry: CRC, sequence, timestamp, finite/range validation, source identity ve sensor quality.

## Health

SAFE -> SEARCHING -> POSSIBLE_TARGET -> WARNING -> CRITICAL -> FAIL_SAFE

Sensör bulunamazsa değer uydurulmaz; unavailable/degraded olarak raporlanır.

## Ham veri ilkesi

Sensors -> telemetry -> validation -> AI Core
             |
             -> storage/replay

AI sonucu raw telemetry'nin yerine geçmez.

## Geliştirme sırası

1. Drone telemetry protocol
2. CRC/sequence/timestamp validation
3. Raw recorder/replay
4. Sensor drivers
5. C++ fusion/risk engine
6. Gemma/llama.cpp
7. Go Bridge authenticated IPC
8. Android Controller
9. Firebase cloud automation
10. Astro website
11. Bench/regression tests
12. Controlled field validation
