# ARES Yazılım ve Sistem Mimarisi

## Uygulama aşaması

Öncelik Deneyap Kart V2 firmware'i, sensör veri edinimi ve doğrulanabilir telemetry sözleşmesidir. MQTT, Windows Ground Station ile telefon/uyumlu cihazlar arasındaki hedef yerel ağ taşımasıdır; güvenli eşleştirme ve otomatik broker kurulumu henüz uygulanmış değildir. Önce sensör verisi ve link davranışı doğrulanır, sonra entegrasyon yapılır.

## Hedef veri hattı

```text
Sensors
  ↓
Deneyap Kart V2 / ESP32 node(s)
  - sensor acquisition
  - timestamps, sequence numbers, status/quality flags
  - basic range/finite checks
  - buffering and SD/raw logging where appropriate
  ↓ validated local link / Wi-Fi after hardware validation
Windows Ground Station
  - packet validation, raw recorder/replay
  - filtering, windowing, feature extraction, sensor fusion
  - Gemma 4 E2B Q5_K_M via llama.cpp
  - local API + Mosquitto broker lifecycle/pairing service (planned)
  ↓ authenticated TLS + MQTT metadata/commands
Paired Android operator app (planned)
```

Bu şema hedef mimaridir; çalışan, uçtan uca entegre edilmiş sistem olarak değerlendirilmemelidir. Companion host donanımı henüz kesinleştirilmemiştir.

## Katman sorumlulukları

### Firmware / telemetry

Deneyap Kart V2 sensör okumalarını edinir, her ölçüme timestamp/sequence ve validity/status bilgisi ekler, veri sınırlarını kontrol eder ve veri kaybını görünür kılar. İki kartın görev dağılımı sensör bus'ları, GPIO ve zamanlama doğrulamasından sonra belirlenir. Firmware Gemma ağırlıklarını çalıştırmaz.

### Raw storage and replay

Ham ölçümler ve türetilmiş özellikler birbirinden ayrılır. Ham kayıtlar kalibrasyon, model karşılaştırması ve hata ayıklama için korunur. SD kart yazımı veri kaybı ve zamanlama açısından test edilir. Ağ kesintisinde kayıt yerelde sürer.

### Windows Ground Station / AI Core

C++ AI Core, Windows veya uyumlu companion host üzerinde çalışacak şekilde hedeflenir. İşlem hattı:

validated telemetry → time alignment/windowing → feature extraction → deterministic sensor fusion → Gemma 4 E2B Q5_K_M → versioned advisory result

Model girişine bütün ham yüksek hızlı akışları kontrolsüz şekilde yığmak yerine doğrulanmış özellikler, sensör kalite bayrakları ve gerekiyorsa seçilmiş görüntü/akustik segmentleri verilir. Gemma kanıtları yorumlayan katmandır; deterministik kontrollerin ve ham kayıtların yerini almaz.

### MQTT, pairing and credential lifecycle

Windows Ground Station yerel Mosquitto broker'ını yönetir. Broker yoksa uygulama görünür/onaylı bir kurulum akışı başlatır, sürümü ve servis sağlığını doğrular, güvenli yapılandırma uygular ve yalnızca tüm kontroller geçerse hazır duruma geçer. Mevcut Mosquitto yapılandırması izinsiz ezilmez.

Telefon eşleştirmesinde Windows'ta gösterilen altı haneli tek kullanımlık PIN, TLS ile korunan pairing endpoint'inde doğrulanır. PIN 120 saniyede sona erer, bir kez kullanılabilir ve hatalı denemeler sınırlandırılır. Başarılı eşleştirmeden sonra Windows cihaza özel, yüksek entropili MQTT kimlik bilgisi üretir ve sadece gerekli topic'lere ACL tanımlar. PIN, MQTT anahtarı değildir. Her telefon/ESP32 düğümü ayrı kimlik alır; ortak anahtar kullanılmaz. Ayrıntılı güvenlik şartları: [MQTT pairing specification](security/mqtt-pairing.md).

SPARK anahtar kasası/şifreleme sistemi değildir. SPARK ile bazı durum makinesi ve komut kabul invariants'ları biçimsel olarak ifade edilebilir; gizli anahtarlar ise doğrulanmış TLS, işletim sistemi güvenli depolaması ve platformun desteklediği korumalarla yönetilmelidir. Deneyap kartında secure boot/flash encryption ve güvenli ilk kayıt mekanizması doğrulanana kadar fiziksel anahtar koruması iddia edilmez.

### MQTT topic ve mesaj kuralları

- Telemetry: ares/v1/telemetry/<device_id>/...
- Durum: ares/v1/status/<device_id>/...
- Komut: ares/v1/command/<device_id>/...
- Her cihaz yalnızca kendi telemetry/status alanına yazabilir; yalnızca kendi komut topic'ini dinleyebilir.
- Komutlar sürümlü şema, command ID, son kullanma zamanı, tekrar oynatma/duplicate kontrolü, yetki ve cihaz durumu kontrollerinden geçer. Retained command mesajları reddedilir.
- MQTT; metadata, özetler, AI sonuçları ve komut zarfları içindir. Yüksek hızlı ham audio/termal akışlar broker'a gelişigüzel gönderilmez.
- Broker erişimi varsayılan olarak güvenilir yerel ağ arayüzüyle sınırlıdır; anonymous access kapalı, TLS ve ACL etkin olmalıdır. Güvenlik ön koşullarından biri başarısızsa pairing ve uzaktan komutlar kapalı kalır.

### Gemma ve uçuş güvenliği sınırı

Gemma 4 E2B Q5_K_M GGUF yaklaşık 3,66 GB sınıfındadır ve ESP32 üzerinde çalıştırılmak üzere seçilmemiştir. Yeterli RAM/işlem gücüne sahip bir host ve gerçek inference benchmark'ı gereklidir. Gemma hiçbir koşulda motor PWM, flight stabilization veya donanımsal failsafe üzerinde doğrudan kontrol sahibi değildir. Windows, MQTT veya telefon bağlantısı kaybolduğunda uçuş güvenliği yerleşik uçuş kontrol sisteminin doğrulanmış failsafe davranışına dayanmalıdır.

## Telemetry mesaj sözleşmesi taslağı

```json
{
  "version": 1,
  "node_id": "deneyap-1",
  "sequence": 18452,
  "timestamp_ms": 1780000123456,
  "sensors": {
    "thermal": {"status": "valid", "anomaly_c": 3.2},
    "uwb": {"status": "unavailable", "distance_m": null},
    "acoustic": {"status": "valid", "rms": 0.031},
    "seismic": {"status": "valid", "rms": 0.021},
    "lidar": {"status": "valid", "distance_m": 8.0}
  }
}
```

Bu şema örnektir; firmware'de uygulanmış protokol olduğu iddia edilmez. Eksik veya geçersiz sensör değerleri uydurulmaz; null/status ile ifade edilir. Paket boyutu, CRC, sequence rollover ve timestamp kaynağı protokol belgesinde kesinleştirilmelidir.

## Geliştirme sırası

1. Pinout, güç ve sensör arayüzlerini doğrula.
2. Firmware'de sensör edinimi, durum bayrakları ve timestamp testleri.
3. UART/local link paket formatı, CRC/sequence ve hata enjeksiyonu.
4. SD raw logger ve kayıt tekrar oynatma.
5. Windows telemetry ingest, deterministic validation/fusion ve Gemma host benchmark'ı.
6. MQTT broker lifecycle, TLS, ACL, PIN pairing ve credential revoke/rotate.
7. Telefon uygulaması ve cihaz bazlı provisioning.
8. Entegre bench testleri, ağ kesintisi/failsafe testleri ve kontrollü saha doğrulaması.

## Doğrulama

CI derlemesi yalnızca derleme başarısını gösterir. Sensör haberleşmesi, zaman senkronizasyonu, UWB doğruluğu, termal kalibrasyon, akustik/sismik performans, eşleştirme güvenliği ve kişi tespiti başarımı ayrı ve tekrarlanabilir testlerle doğrulanmalıdır.
