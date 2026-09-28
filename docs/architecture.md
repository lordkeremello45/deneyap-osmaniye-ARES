# ARES yazılım ve sistem mimarisi

ARES; enkaz arama/kurtarma senaryosunda drone üzerindeki sensörlerden gelen verileri toplayan, bunları yerel olarak işleyen ve operatöre anlamlandırılmış sonuçlar sunan katmanlı bir sistemdir.

Temel mimari yaklaşım, HWcontrol2.0'daki **Flutter/Dart → Go Bridge → native C++** ayrımından esinlenir; ancak ARES'in sensör füzyonu, gerçek zamanlı telemetri ve uçuş güvenliği ihtiyaçlarına göre uyarlanır.

> **Önemli sınır:** AI Core uçuş kontrolcüsü değildir. Uçuş, motor, failsafe ve gerçek zamanlı kontrol kararları AI modeline bırakılmaz. AI çıktıları operatör kararını destekleyen analiz sonuçlarıdır.

## 1. Ana mimari

```text
                         ARES SYSTEM
                              │
                    ┌─────────▼─────────┐
                    │   Flutter / Dart  │
                    │    Mobile UI      │
                    │                   │
                    │ Telemetry         │
                    │ Thermal view      │
                    │ Map / position    │
                    │ Alerts            │
                    │ Manual control    │
                    └─────────┬─────────┘
                              │
                     HTTP + WebSocket
                              │
                    ┌─────────▼─────────┐
                    │    Go Bridge      │
                    │                   │
                    │ API / WebSocket   │
                    │ Device manager    │
                    │ Data routing      │
                    │ Logging           │
                    │ Protocol          │
                    └──────┬──────┬─────┘
                           │      │
                       IPC│      │Raw data
                           │      │
              ┌────────────▼─┐  ┌─▼──────────────┐
              │   C++ AI Core│  │ Raw Data       │
              │              │  │ Pipeline       │
              │ Validation   │  │                │
              │ Filtering    │  │ Thermal        │
              │ Sensor fusion│  │ UWB            │
              │ AI analysis  │  │ Acoustic       │
              │ Gemma 3 1B   │  │ IMU/GPS        │
              │ Q5_K_M       │  │ LiDAR          │
              └───────┬──────┘  └──────┬─────────┘
                      │                 │
                      └────────┬────────┘
                               │
                    Structured AI result
                               │
                          Go Bridge
                               │
                         Flutter UI
```

## 2. Katmanların sorumlulukları

### 2.1 Flutter / Dart — mobil kontrol ve görüntüleme

Flutter uygulaması kullanıcı arayüzünü oluşturur.

Sorumluluklar:
- canlı telemetriyi göstermek,
- termal görüntü/veriyi göstermek,
- harita ve konum bilgisini göstermek,
- AI analiz sonuçlarını ve uyarıları göstermek,
- operatörün manuel komutlarını göndermek,
- bağlantı ve sistem durumunu göstermek.

Flutter **sensör verisinin anlamını belirleyen ana katman değildir**. Veriyi Go Bridge'den alır ve kullanıcıya sunar.

### 2.2 Go Bridge — iletişim, yönlendirme ve orkestrasyon

Go Bridge sistemin servis katmanıdır.

Sorumluluklar:
- Flutter ile HTTP/WebSocket iletişimi,
- C++ AI Core ile IPC iletişimi,
- cihaz/telemetri bağlantılarının yönetimi,
- ham verinin ve yapılandırılmış AI sonuçlarının yönlendirilmesi,
- kayıt/log yönetimi,
- bağlantı durumu ve servis sağlığı,
- cihaz protokolünün üst seviye yönetimi.

Go Bridge **AI sonucunu yeniden yorumlamaz**. C++ Core tarafından üretilen yapılandırılmış sonucu taşır, kaydeder ve UI'ya sunar.

Önerilen MVP iletişimi:
- Flutter ↔ Go: HTTP + WebSocket
- Go ↔ C++: yerel IPC
- yüksek frekanslı sensör akışları: gerektiğinde binary frame
- olay/analiz mesajları: versioned JSON veya benzeri yapılandırılmış mesaj

### 2.3 C++ AI Core — sensör işleme ve yapay zekâ

AI Core sistemin analiz katmanıdır.

Sorumluluklar:
1. sensör verisini almak,
2. veri doğrulama yapmak,
3. filtreleme ve normalizasyon yapmak,
4. zaman pencereleri oluşturmak,
5. sensör füzyonu yapmak,
6. anlamlı özellikleri çıkarmak,
7. Gemma 3 1B Q5_K_M ile analiz yapmak,
8. sonucu Go'nun doğrudan kullanabileceği yapılandırılmış formata çevirmek.

Önemli tasarım ilkesi:

**Ham yüksek frekanslı sensör akışı doğrudan Gemma'ya verilmez.**

Önce deterministik preprocessing/fusion katmanı çalışır. Model; örneğin sıcaklık anomalisi, hareket özelliği, UWB mesafe/konum bilgisi, akustik özellikler ve zaman penceresi gibi özetlenmiş yapılandırılmış veriler üzerinde çalışır.

Örnek iç akış:

```text
Sensor input
    ↓
Validation
    ↓
Filtering / normalization
    ↓
Time window
    ↓
Sensor fusion
    ↓
Feature extraction
    ↓
Gemma 3 1B Q5_K_M
    ↓
Structured analysis result
```

### 2.4 Raw Data Pipeline — ham verinin korunması

AI sonucu ham sensör verisinin yerine geçmez.

Ham veri ayrı bir akışta korunmalıdır:

```text
Sensors
   ├──→ Raw Data Pipeline → logging / replay / diagnostics
   │
   └──→ AI Core → analysis → structured result
```

Bunun amacı:
- sonradan tekrar analiz yapabilmek,
- model sonuçlarını doğrulayabilmek,
- sensör füzyonunu test edebilmek,
- hata ayıklama ve regresyon testi yapabilmek,
- AI çıktısının kaynağını izleyebilmek.

## 3. AI Core çıktı sözleşmesi

AI Core, Go'ya serbest metin yerine makine tarafından işlenebilir bir sonuç göndermelidir.

Örnek:

```json
{
  "version": 1,
  "type": "human_presence",
  "status": "possible",
  "confidence": 0.87,
  "sources": [
    "thermal",
    "uwb",
    "acoustic"
  ],
  "position": {
    "x": 2.41,
    "y": 1.83
  },
  "timestamp": 1727550000
}
```

Bu değerler yalnızca örnektir; gerçek eşikler ve alanlar sensör validasyonu sonrasında belirlenecektir.

Önerilen temel alanlar:
- `version`: mesaj şema sürümü,
- `type`: olay/analiz türü,
- `status`: analiz durumu,
- `confidence`: model/füzyon güven değeri,
- `sources`: sonucu destekleyen sensörler,
- `position`: varsa tahmini konum,
- `timestamp`: ölçüm/analiz zamanı.

Gerekirse buna `raw_reference`, `sensor_quality`, `model_version` ve `processing_time_ms` gibi izlenebilirlik alanları eklenebilir.

## 4. Cihaz ve firmware katmanı

Drone üzerindeki Deneyap Kart düğümleri sensörlerin gerçek zamanlı okunması ve cihaz protokolünün uygulanmasından sorumludur.

Önerilen yapı:

```text
firmware/
├── common/
│   ├── framing
│   ├── crc
│   ├── messages
│   └── time
├── node_a/
├── node_b/
└── drivers/
```

Firmware sorumlulukları:
- sensör sürücüleri,
- örnekleme,
- timestamp,
- veri doğrulama,
- UART/uygun fiziksel katman haberleşmesi,
- watchdog,
- cihaz failsafe davranışları,
- paketleme ve CRC.

Mevcut iki düğümlü UART yaklaşımı korunur. Uzun veya elektromanyetik olarak gürültülü hatlarda diferansiyel fiziksel katman/RS-485 değerlendirilir.

## 5. Haberleşme sınırları

### Flutter ↔ Go

Gerçek zamanlı telemetri ve olaylar için WebSocket; konfigürasyon, health ve kontrol API'leri için HTTP kullanılabilir.

### Go ↔ C++

Yerel IPC tercih edilir.

MVP'de platform ihtiyacına göre:
- Linux: Unix domain socket,
- Windows: named pipe,
- ortak ve basit çapraz platform gereksiniminde localhost TCP.

gibi seçenekler değerlendirilebilir.

gRPC ancak mesaj sözleşmesi ve servis sayısı büyüdüğünde değerlendirilmelidir; MVP'de gereksiz karmaşıklık eklenmemelidir.

### Firmware ↔ host

UART başlangıç seçeneğidir.

Frame yapısı mevcut protokol taslağıyla uyumludur:

```text
SYNC | VERSION | SRC | DST | TYPE | SEQ | LENGTH | PAYLOAD | CRC16
```

CRC16-CCITT-FALSE, sequence number, ACK/NACK, timeout/retry ve gerektiğinde zaman senkronizasyonu kullanılmalıdır.

## 6. Uçuş kontrolü ve AI sınırı

AI Core aşağıdaki kararların sahibi olmamalıdır:
- motor PWM üretimi,
- doğrudan uçuş kontrolü,
- stabilizasyon,
- acil motor kesme,
- uçuş failsafe,
- otonom kurtarma kararı.

Bu işlevler uçuş kontrolcüsü/firmware ve operatör kontrolü altında kalmalıdır.

AI Core'un görevi:

```text
Sensörler
   ↓
AI Core
   ↓
"olası insan varlığı / termal anomali / veri güveni ..."
   ↓
Go Bridge
   ↓
Flutter
   ↓
Operatör değerlendirmesi
```

## 7. Önerilen repository yapısı

```text
ARES/
├── ai_core/
│   ├── include/
│   ├── src/
│   ├── tests/
│   └── CMakeLists.txt
│
├── bridge_service/
│   ├── main.go
│   ├── internal/
│   └── go.mod
│
├── firmware/
│   ├── common/
│   ├── node_a/
│   ├── node_b/
│   └── drivers/
│
├── mobile_app/
│   ├── lib/
│   ├── test/
│   └── pubspec.yaml
│
├── models/
│   └── gemma-3-1b-it-Q5_K_M.json
│
├── tools/
├── tests/
├── docs/
└── site/
```

Bu yapı HWcontrol2.0'ın dil/katman ayrımını ARES'e taşır:
- **Dart/Flutter:** UI
- **Go:** bridge/service
- **C++/CMake:** sensor processing + fusion + AI Core
- **C/C++:** firmware
- **Gemma 3 1B Q5_K_M:** AI model

## 8. Geliştirme sırası

1. Ölçülebilir gereksinimleri tanımla: payload, endurance, menzil, çevre, kablo, baud rate, güç ve güvenlik sınırları.
2. Sensör ve donanım datasheet/uyumluluk araştırmasını tamamla.
3. Firmware sensör sürücülerini ve UART protokolünü doğrula.
4. Ham veri pipeline'ını oluştur.
5. C++ AI Core'da validation/filtering/fusion katmanını oluştur.
6. Gemma entegrasyonunu yapılandırılmış feature input ile ekle.
7. AI Core → Go yapılandırılmış mesaj sözleşmesini sabitle.
8. Go Bridge → Flutter WebSocket/HTTP katmanını ekle.
9. Ham veri kayıt/replay ve AI regresyon testlerini ekle.
10. Sensörleri tek tek entegre et ve her aşamada regresyon testi çalıştır.
11. Bench test → pervaneler sökülü sistem testi → kontrollü ve izinli saha testi sırasını izle.

## 9. Güvenilirlik ve test

Her katman bağımsız test edilebilir olmalıdır.

### AI Core
- sensör verisi doğrulama,
- filtreleme,
- füzyon,
- aynı girdide tekrarlanabilir sonuç,
- model yükleme/bozulma testi,
- gecikme ölçümü,
- yanlış pozitif/yanlış negatif değerlendirmesi.

### Go Bridge
- bağlantı kopması,
- yeniden bağlanma,
- mesaj sıralaması,
- timeout,
- backpressure,
- log bütünlüğü.

### Flutter
- bağlantı durumu,
- eksik/gecikmiş telemetri,
- AI olaylarının doğru gösterimi,
- operatör komutlarının doğrulanması.

### Firmware
- CRC,
- sequence,
- ACK/retry,
- watchdog,
- sensör hata durumları,
- EMI altında haberleşme.

## 10. Açık teknik kararlar

- Kesin sensör modelleri ve sensör füzyon algoritmaları.
- UART baud rate ve fiziksel katman.
- Go ↔ C++ IPC yönteminin kesin seçimi.
- Flutter ↔ Go mesaj şemasının kesinleşmesi.
- AI Core çıktı JSON şemasının sürümlendirilmesi.
- Raw data depolama formatı ve saklama süresi.
- Gemma inference context/window boyutları.
- Model quantization ve performans hedefleri.
- Uçuş kontrolcüsü ile ARES yazılımı arasındaki kesin sınır.
- Güç bütçesi, işlemci yükü ve termal sınırlar.

## 11. Temel mimari prensipler

1. **AI Core her şeyi yönetmez.**
2. **AI Core sensörleri okur, işler, füzyonlar ve değerlendirir.**
3. **Go Bridge taşıma, servis ve orkestrasyon katmanıdır; AI sonucunu yeniden yorumlamaz.**
4. **Flutter kullanıcı arayüzüdür.**
5. **Ham sensör verisi AI sonucundan bağımsız korunur.**
6. **Gemma'ya ham yüksek frekanslı akış yerine yapılandırılmış özellikler verilir.**
7. **Uçuş güvenliği AI modeline bırakılmaz.**
8. **Her mesaj ve servis sürümlenebilir ve test edilebilir olmalıdır.**
9. **Donanım ve yazılım kararları bench testleriyle doğrulanmadan kesin kabul edilmez.**
