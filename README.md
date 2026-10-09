# ARES — Deneyap Osmaniye

Drone destekli enkaz arama/kurtarma araştırma ve prototip sistemi.

## Mevcut geliştirme önceliği

Önce Deneyap Kart V2 firmware'i, sensör edinimi, zaman damgalı telemetry ve ham veri kaydı geliştirilecek. Windows Ground Station, yerel AI/fusion ve daha sonra operatör bağlantısı için hedef host katmanıdır. Mobil uygulama ve MQTT pairing henüz uygulanmış uçtan uca özellikler değildir.

## Hedef mimari

```text
Sensors
   ↓
Deneyap Kart V2 / ESP32
acquisition + timestamps + status checks + SD logging
   ↓ validated local link / Wi-Fi after hardware validation
Windows Ground Station
telemetry validation + sensor fusion + Gemma 4 E2B Q5_K_M
local API + Mosquitto broker + secure pairing (planned)
   ⇅ authenticated TLS / MQTT
Android operator app (planned)
```

Windows Ground Station hedefi; Mosquitto yoksa açık/onaylı kurulum akışıyla kurulum, servis/konfigürasyon doğrulaması ve güvenli yapılandırma; ardından telefon için altı haneli, 120 saniyede sona eren, tek kullanımlık PIN ile eşleştirme ve her cihaz için benzersiz MQTT kimlik bilgisi üretimidir. PIN, MQTT key değildir. Broker anonymous erişime kapalı olmalı; TLS, cihaz başına topic ACL, credential revoke/rotate ve rate limiting zorunludur. Ayrıntılar: [MQTT pairing security specification](docs/security/mqtt-pairing.md).

**SPARK bir anahtar kasası değildir.** SPARK bazı komut/mission state kurallarının biçimsel tanımlanmasına yardımcı olabilir; anahtarların gizliliği doğrulanmış kriptografi ve işletim sistemi/platform güvenli depolamasıyla sağlanır. Deneyap kartındaki güvenli provisioning ve flash koruması doğrulanmadan fiziksel anahtar koruması iddia edilmez.

## Gemma'nın rolü ve donanım sınırı

Seçilen model Gemma 4 E2B Instruct Q5_K_M GGUF'tur. Yaklaşık 3,66 GB model dosyası ve çıkarım belleği nedeniyle Deneyap Kart V2/ESP32 üzerinde çalıştırılmak üzere seçilmemiştir. Yeterli RAM/işlem gücüne sahip companion host ve gerçek inference benchmark'ı gerekir. Model, doğrulanmış sensör özelliklerini ve kalite bayraklarını yorumlayan yardımcı katmandır; ham veri kayıtlarının ve deterministik kontrollerin yerini almaz.

Gemma; uçuş stabilizasyonu, motor PWM veya donanımsal failsafe kontrol etmez. Ağ/Windows/telefon bağlantısı kaybında uçuş emniyeti doğrulanmış yerleşik uçuş kontrol sistemi tarafından sağlanmalıdır.

## Yazılım alanları

- `firmware/` — Deneyap Kart V2 / ESP32 sensör edinimi ve veri protokolü.
- `ai_core/` — companion host üzerinde C++ analiz; Gemma adapter'ı tamamlanmış kabul edilmez.
- `models/` — model metadata ve yerel model yolu; ağırlık dosyası Git'e eklenmez.
- `bridge_service/` — Go servis/API başlangıç iskeleti; şu anda tam telemetry/pairing servisi değildir.
- `mobile_app/` — planlanan Flutter Android/iOS operatör uygulaması.
- `docs/ada-spark/` — ayrı Ada/SPARK doğrulama çalışması.
- `docs/security/mqtt-pairing.md` — hedef pairing, credential lifecycle, Mosquitto ve güvenlik gereksinimleri.
- `site/` — GitHub Pages tanıtım sitesi.

## Sensör ve donanım sınırları

Pinout, breakout kartları, güç bütçesi ve elektriksel arayüzler fiziksel olarak doğrulanmadan sabitlenmez. XMOS XVF3800 arayüzü, DWM3000 sürücüsü ve Geospace sensörü için analog ön uç gibi bağımlılıklar ayrıca çözülmelidir. Ayrıntılar: [BOM](docs/hardware/bom.md) ve [sensör kütüphaneleri](docs/hardware/sensor-libraries.md).

## Lisanslar

- Ana proje kaynak kodu: [GPL-3.0](LICENSE)
- Yalnızca `site/` ve `bridge_service/` kapsamındaki kaynaklar için ek lisans: [AGPL-3.0](LICENSE-AGPL-3.0)
- Bileşen kapsamı: [LICENSES.md](LICENSES.md)
- Gemma model ağırlıkları ARES kaynak kodu lisanslarından ayrı Google Gemma şartlarına tabidir.

## Geliştirme komutları

```sh
cmake -S ai_core -B ai_core/build
cmake --build ai_core/build

cd bridge_service
go run .
```

Derleme, sensör haberleşmesinin, MQTT güvenliğinin veya kişi tespit başarımının doğrulandığı anlamına gelmez.

## Durum

ARES prototiptir. Sensör sonuçları operatör destek verisidir ve gerçek arama/kurtarma operasyonlarında sertifikalı sistemlerin yerine geçmez.

## Project and community

- [Contributors](CONTRIBUTORS.md) — project team and contribution credits.
- [Contributing](CONTRIBUTING.md) — development workflow, testing, and pull request expectations.
- [Code of Conduct](CODE_OF_CONDUCT.md) — collaboration standards.
- [Security Policy](SECURITY.md) — responsible vulnerability reporting.
- [Support](SUPPORT.md) — issue reporting and diagnostic information.
- [Changelog](CHANGELOG.md) — notable project changes.
- [Source bibliography](source.md) — consolidated hardware/materials inventory, official datasheets, software dependencies, and open verification items.
- [GitHub contributors graph](https://github.com/lordkeremello45/deneyap-osmaniye-ARES/graphs/contributors) — contributions recorded by GitHub.

