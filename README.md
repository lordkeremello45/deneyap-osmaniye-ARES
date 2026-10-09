# ARES — Deneyap Osmaniye

Drone destekli enkaz arama/kurtarma araştırma ve prototip sistemi.

## Mevcut geliştirme önceliği

**Önce Deneyap Kart V2 firmware'i ve sensör veri hattı geliştirilecek. Mobil uygulama henüz geliştirme aşamasında değildir.** İlk hedef, sensörleri güvenli biçimde okumak, zaman damgalı veriyi doğrulamak/kaydetmek ve Gemma analizine uygun bir veri sözleşmesi üretmektir.

## Hedef mimari

```text
FLIR Lepton 3.5 ─┐
Qorvo DWM3000 ───┤
XMOS XVF3800 ────┤
Geospace GS-One ─┤
Garmin LIDAR-Lite┤
                 ▼
       Deneyap Kart V2 / ESP32
       acquisition + timestamps
       range/status checks + buffering
                 │
          UART / local link
                 ▼
       Companion host computer
       raw logging + feature fusion
       Gemma 4 E2B Q5_K_M (llama.cpp)
                 │
       structured advisory results
                 ▼
       Future operator interface
       Deneyap BT / BLE or MQTT
       transport to be selected and tested
```

İkinci Deneyap Kart V2, sensör gruplarını ayırma veya görevleri bölme amacıyla kullanılabilir; rolü pin bütçesi, bus yükü ve zamanlama testlerinden sonra kesinleştirilecektir. Mobil uygulama, bağlantı protokolü ve uzaktan izleme sonraki aşamadır.

## Gemma'nın rolü ve donanım sınırı

Seçilen model **Gemma 4 E2B Instruct Q5_K_M GGUF**. Yaklaşık 3,66 GB model dosyası ve çıkarım belleği nedeniyle bunu Deneyap Kart V2/ESP32 üzerinde çalıştırmayı hedeflemiyoruz. Model, yeterli RAM/işlem gücüne sahip bir companion host üzerinde `llama.cpp` ile çalıştırılmak üzere seçildi. Bu host henüz BOM'da kilitlenmiş bir donanım değildir; model entegrasyonundan önce seçilmesi gerekir.

Gemma'ya her sensörün ham yüksek frekanslı akışı gelişigüzel gönderilmeyecek. Firmware ham ölçümleri, zaman damgalarını, kalite/durum bayraklarını ve mümkün olan ham kayıtları korur. Host tarafı doğrulama, pencereleme ve sensör füzyonundan sonra anlamlı yapılandırılmış özellikleri ve gerektiğinde seçilmiş termal kare/akustik özetlerini Gemma'ya sunar. Böylece Gemma çoklu sensör kanıtlarını yorumlayabilir; ham veri ve deterministik güvenlik kontrolleri kaybolmaz.

Gemma sonucu operatör destek bilgisidir. Uçuş stabilizasyonu, motor PWM ve donanımsal failsafe kararları LLM'ye bırakılmaz. Gerçek kişi tespiti başarımı ancak etiketli ve tekrarlanabilir deneylerle değerlendirilebilir.

## Yazılım alanları

- `firmware/` — şu anki öncelik: Deneyap Kart V2 / ESP32 sensör edinimi ve veri protokolü
- `ai_core/` — companion host üzerinde C++ analiz ve ileride llama.cpp/Gemma entegrasyonu
- `models/` — model metadata ve yerel model yolu; ağırlık dosyası Git'e eklenmez
- `bridge_service/` — sonraki aşamada yerel servis/API; şu anda sensör verisini gerçekten yönlendiren bir uçtan uca sistem olduğu varsayılmaz
- `mobile_app/` — planlanan Flutter Android/iOS operatör uygulaması; henüz bu aşamanın odağı değil
- `docs/ada-spark/` — ayrı Ada/SPARK doğrulama çalışması
- `site/` — yalnızca GitHub Pages tanıtım sitesi

## Sensör ve donanım sınırları

Kesin pinout, breakout kartları, güç bütçesi ve elektriksel arayüzler fiziksel olarak doğrulanmadan sabitlenmez. XMOS XVF3800'ün seçilen firmware/arayüzü, DWM3000 sürücüsü ve Geospace sensörü için analog ön uç gibi bağımlılıklar ayrıca çözülmelidir. Ayrıntılar: [BOM](docs/hardware/bom.md) ve [sensör kütüphaneleri](docs/hardware/sensor-libraries.md).

## Model dosyası

Model yolu, indirme kaynağı ve ayrı Gemma şartları için [model runtime notlarına](models/README.md) bakın. Model ağırlıkları repoya eklenmemiştir; gerçek inference adapter'ı tamamlanmış veya test edilmiş kabul edilmez.

## Ada/SPARK

`docs/ada-spark/` sensör ölçümlerini kontrol etmek ve ihtiyatlı durum seçimi için ayrı bir başlangıçtır. GNATprove sonucu alınana kadar biçimsel doğrulama başarılı kabul edilmez.

## Lisanslar

- Ana proje kaynak kodu: [GPL-3.0](LICENSE)
- Yalnızca `site/` ve `bridge_service/` kapsamındaki kaynaklar için ek lisans: [AGPL-3.0](LICENSE-AGPL-3.0)
- Bileşen kapsamı: [LICENSES.md](LICENSES.md)
- Gemma model ağırlıkları, ARES kaynak kodu lisanslarından ayrı Google Gemma şartlarına tabidir.

## Geliştirme komutları

```sh
# C++ core compile check
cmake -S ai_core -B ai_core/build
cmake --build ai_core/build

# Go bridge (later integration stage)
cd bridge_service
go run .
```

Firmware için PlatformIO ve [firmware README](firmware/README.md) kullanılmalıdır. Derleme, sensör haberleşmesinin veya kişi tespit başarımının doğrulandığı anlamına gelmez.

## Durum

ARES prototiptir. Sensör sonuçları operatör destek verisidir ve gerçek arama/kurtarma operasyonlarında sertifikalı sistemlerin yerine geçmez.
