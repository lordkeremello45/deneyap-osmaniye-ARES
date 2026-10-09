# ARES Yazılım ve Sistem Mimarisi

## Uygulama aşaması

Öncelik şu anda **Deneyap Kart V2 firmware'i, sensör veri edinimi ve doğrulanabilir telemetry sözleşmesidir**. Mobil uygulama henüz geliştirme odağı değildir. Bluetooth/Deneyap BT veya MQTT seçimi sonraki aşamaya bırakılmıştır; bağlantı kararı ölçüm ve menzil testleriyle verilecektir.

## Hedef veri hattı

```text
Sensors
  ↓
Deneyap Kart V2 / ESP32 node(s)
  - sensor acquisition
  - timestamps, sequence numbers, status/quality flags
  - basic range/finite checks
  - buffering and SD/raw logging where appropriate
  ↓ UART or a separately validated local link
Companion host computer (hardware TBD)
  - packet validation and raw recorder/replay
  - filtering, windowing, feature extraction, sensor fusion
  - Gemma 4 E2B Q5_K_M via llama.cpp
  ↓ structured advisory result
Future transport/interface (TBD)
  - Deneyap BT/BLE or MQTT over Wi-Fi
  - later Android/iOS controller
```

Bu şema hedef mimaridir; henüz çalışan, uçtan uca entegre edilmiş bir sistem olarak değerlendirilmemelidir. Companion host donanımı henüz seçilmemiştir.

## Katman sorumlulukları

### Firmware / telemetry

Deneyap Kart V2 sensör okumalarını edinir, her ölçüme timestamp/sequence ve validity/status bilgisi ekler, veri sınırlarını kontrol eder ve veri kaybını görünür kılar. İki kartın görev dağılımı sensör bus'ları, GPIO ve zamanlama doğrulamasından sonra belirlenir. Firmware Gemma ağırlıklarını çalıştırmaz.

### Raw storage and replay

Ham ölçümler ve türetilmiş özellikler birbirinden ayrılır. Ham kayıtlar kalibrasyon, model karşılaştırması ve hata ayıklama için korunur. SD kart yazımı veri kaybı ve zamanlama açısından test edilir.

### Companion host / AI Core

C++ AI Core, ESP32'nin dışında çalışacak bir host bilgisayar hedefler. İşlem hattı:

`validated telemetry → time alignment/windowing → feature extraction → deterministic sensor fusion → Gemma 4 E2B Q5_K_M → versioned result`

Model girişine bütün ham yüksek hızlı akışları kontrolsüz şekilde yığmak yerine doğrulanmış özellikler, sensör kalite bayrakları ve gerekiyorsa seçilmiş görüntü/akustik segmentleri verilir. Gemma çoklu sensör kanıtlarını yorumlayan katmandır; deterministik kontrollerin ve ham kayıtların yerini almaz. Model sonucu güven düzeyi ve kanıt kaynaklarıyla birlikte raporlanmalı, doğrulanmamış olasılıklar gerçek tespit olarak sunulmamalıdır.

### Gemma ve güvenlik sınırı

Gemma 4 E2B Q5_K_M GGUF yaklaşık 3,66 GB sınıfındadır ve ESP32/Deneyap Kart V2 üzerinde çalıştırılmak üzere seçilmemiştir. `llama.cpp` çalıştırabilecek yeterli RAM ve işlem gücüne sahip companion host gereklidir. Host seçimi ve model benchmark'ı açık gereksinimlerdir.

Gemma hiçbir koşulda motor PWM, flight stabilization veya donanımsal failsafe üzerinde doğrudan kontrol sahibi değildir. Bu fonksiyonlar deterministik, test edilebilir kontrol sistemlerinde kalır.

### İletişim ve gelecek mobil katmanı

- İlk etapta ESP32 telemetry'sini USB/UART/seri günlükleriyle doğrula.
- Companion host bağlantısı için UART veya başka bir yerel link yalnızca elektriksel/performans testlerinden sonra sabitlenir.
- Mobil operatör arayüzü daha sonraki aşamadır.
- Deneyap BT/BLE ve MQTT alternatifleridir, kesin karar değildir. BLE düşük hacimli yakın alan telemetrisi için; MQTT ise Wi-Fi/IP ağı, broker güvenliği ve bağlantı sürekliliği gerektiğinde değerlendirilebilir. Nihai seçim güç, gecikme, menzil, paket kaybı ve saha ağ koşullarına göre yapılır.

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

Bu şema örnektir; firmware'de uygulanmış protokol olduğu iddia edilmez. Eksik veya geçersiz sensör değerleri uydurulmaz; `null`/status ile ifade edilir. Paket boyutu, endian, CRC, sequence rollover ve timestamp kaynağı protokol belgesinde kesinleştirilmelidir.

## Geliştirme sırası

1. Gerçek pinout, besleme ve sensör arayüzlerini doğrula.
2. Firmware'de tek sensörlü edinim, durum bayrakları ve timestamp testleri.
3. UART paket formatı, CRC/sequence ve hata enjeksiyonu.
4. SD raw logger ve kayıt tekrar oynatma.
5. İki Deneyap kartının görev ayrımı ve bus yükü testleri.
6. Companion host donanımını seç; RAM/CPU/GPU/enerji bütçesini belirle.
7. C++ validation, windowing ve fusion testleri.
8. llama.cpp/Gemma model adapter'ı ve gecikme/RAM benchmark'ı.
9. Yerel link ve daha sonra BLE/MQTT seçeneklerinin ölçümlü karşılaştırması.
10. Android/iOS arayüzü.
11. Entegre bench ve saha doğrulaması.

## Doğrulama

CI derlemesi yalnızca derleme başarısını gösterir. Sensör haberleşmesi, zaman senkronizasyonu, UWB doğruluğu, termal kalibrasyon, akustik/sismik performans ve kişi tespiti etiketli kontrollü testlerle ayrı ayrı doğrulanmalıdır.
