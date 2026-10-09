# ARES — Kaynakça, Malzeme Envanteri ve Araştırma Kaydı

> **Belge türü:** Birleştirilmiş teknik kaynakça ve araştırma indeksi  
> **Son gözden geçirme:** 9 Ekim 2026  
> **Ana uygulama deposu:** [deneyap-osmaniye-ARES](https://github.com/lordkeremello45/deneyap-osmaniye-ARES)  
> **Referans deposu:** [HWcontrol2.0](https://github.com/lordkeremello45/HWcontrol2.0)

Bu dosya ARES için proje kayıtlarında adı geçen donanım, yazılım, kütüphane, güvenlik tasarımı ve malzemelere ait kaynakları tek yerde toplar. Üretici veri sayfaları birincil kaynak kabul edilir; mağaza sayfaları ancak üretici belgesi yerine geçmediği durumlarda yardımcı kaynak olarak kullanılır.

**Önemli durum ayrımı**
- **Envanterde / konuşulmuş:** Proje için adı geçen veya elde olduğu bildirilen parça.
- **Aday:** Tam model, breakout kartı, revizyon veya uyumluluk henüz doğrulanmamış olabilir.
- **Yazılım bağımlılığı:** Depoda belirtilen veya manifestte kullanılan yazılım/kütüphane.
- **Doğrulanmadı:** Fiziksel test, kalibrasyon, sürüm kontrolü veya üretici dokümanı karşılaştırması gerekiyor.
- Bu kaynakçada bir ürünün bulunması; satın alındığı, doğru bağlandığı, uçuşa uygun olduğu veya ARES üzerinde çalıştığı anlamına **gelmez**.

---

## 1. Proje depoları ve kurum içi teknik belgeler

| Kaynak | Amaç / kapsam |
|---|---|
| [ARES ana deposu](https://github.com/lordkeremello45/deneyap-osmaniye-ARES) | Uygulama için tek doğruluk kaynağı; mevcut kod, yapılandırma ve durum burada incelenmelidir. |
| [HWcontrol2.0 referans deposu](https://github.com/lordkeremello45/HWcontrol2.0) | Dil, framework ve mimari örüntülerini incelemek için referans; ARES uygulama durumunun kaynağı değildir. |
| [README.md](README.md) | Hedef mimari, güncel öncelikler, Gemma'nın rolü ve uygulama sınırları. |
| [docs/architecture.md](docs/architecture.md) | Veri hattı, firmware, telemetry, AI Core, Windows Ground Station ve güvenlik sınırları. |
| [docs/hardware/bom.md](docs/hardware/bom.md) | Malzeme adayları ve donanım doğrulama gereksinimleri. |
| [docs/hardware/sensor-libraries.md](docs/hardware/sensor-libraries.md) | Sensör kütüphaneleri, sürümleri ve entegrasyon sınırlamaları. |
| [docs/research/decision-log.md](docs/research/decision-log.md) | Açık teknik kararlar ve araştırma yöntemi. |
| [docs/security/mqtt-pairing.md](docs/security/mqtt-pairing.md) | MQTT kimlik, TLS, ACL, pairing ve credential yaşam döngüsü için tasarım gereksinimleri. |
| [docs/security/bluetooth-first-setup.md](docs/security/bluetooth-first-setup.md) | Windows üzerinden ilk kayıt ve Bluetooth provisioning tasarımı. |
| [docs/validation/windows.md](docs/validation/windows.md) | Windows preflight/CI kapsamı ve doğrulanmayan alanlar. |
| [CONTRIBUTORS.md](CONTRIBUTORS.md) | Ekip ve görev dağılımı. |
| [LICENSES.md](LICENSES.md) | ARES kaynak kodu, alt dizin lisansları ve üçüncü taraf model ağırlıklarının lisans sınırları. |

### Araştırma kayıt yöntemi

1. Kesin üretici parça numarası ve revizyonu belirle.
2. Üretici veri sayfasını ve uygulama notlarını kaydet.
3. Besleme aralığı, mantık seviyesi, arayüz, tepe/ortalama akım, boyut ve kütleyi doğrula.
4. Sürücü lisansı ve tam sürüm/commit bilgisini sabitle.
5. Önce tezgâh testi, sonra bütünleşik test yap; ham ölçümleri ve test kurulumunu kaydet.
6. Kararı, varsayımları, reddedilen alternatifleri ve test kanıtını [decision-log.md](docs/research/decision-log.md) içine işle.

---

## 2. Donanım ve malzeme envanteri

### 2.1 Kontrol, depolama ve mantık

| Parça | Kaynak | Teknik not / açık doğrulama |
|---|---|---|
| **2 × DENEYAP Kart V2** | [Resmî ürün sayfası](https://magaza.deneyapkart.org/tr/product/detail/deneyap-kart-v2-type-c) | Ürün sayfası ESP32-S3-WROOM-1-N8R8, 8 MB Octal SPI PSRAM, 8 MB Flash ve Bluetooth LE 5/Mesh belirtir. Kartın tam revizyonu, üzerindeki modül etiketi ve gerçek pinout'u fiziksel olarak teyit edilmeli. |
| **32 GB SD kart** | [Espressif Arduino-ESP32 SD kütüphanesi](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD) · [SD Association](https://www.sdcard.org/) | Marka/model, hız sınıfı, kart tipi ve dayanıklılık sınıfı henüz kayda geçirilmemiş. Uzun süreli telemetry kaydı için gerçek yazma hızı, güç kesintisi davranışı, dosya sistemi ve kart ömrü test edilmeli. |
| **74LVC2G17** | [Nexperia ürün sayfası ve datasheet](https://www.nexperia.com/products/analog-logic-ics/logic/buffers-inverters-transceivers/schmitt-trigger-ics/serie/74lvc2g17/) | İki kanallı, Schmitt-trigger girişli, non-inverting buffer. 1.65–5.5 V besleme aralığı ve 5 V toleranslı giriş bilgisi üretici sayfasında bulunur. **İki yönlü level shifter değildir**; VCC, giriş/çıkış seviyeleri ve yük şartları tasarıma göre kontrol edilmeli. |
| **DENEYAP Kart üzerindeki depolama / SD arayüzü** | [Resmî kart sayfası](https://magaza.deneyapkart.org/tr/product/detail/deneyap-kart-v2-type-c) · [Espressif SD sürücüsü](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD) | Kart sayfası microSD okuyucu pinlerini belirtir. Kart üzerindeki okuyucu kullanılırsa ek SD breakout varsayılmamalı; tam kart pinout'u ve sürücü modu doğrulanmalı. |

### 2.2 Termal, akustik, titreşim ve mesafe algılama

| Parça | Birincil kaynak | Teknik not / açık doğrulama |
|---|---|---|
| **FLIR Lepton 3.5** | [Teledyne FLIR OEM — Lepton ve teknik belgeler](https://oem.flir.com/products/lepton/) · [Lepton Arduino kütüphanesi](https://github.com/NachtRaveVL/Lepton-FLiR-Arduino) | Lepton 3.5 radyometrik termal görüntüleme için değerlendirilir. Breakout kartı, güç ve SPI/VoSPI zamanlaması, I²C CCI, radyometri yapılandırması ve kalibrasyon referansı doğrulanmalı. Termal anomali tek başına enkaz altında insan bulunduğunu kanıtlamaz. |
| **Qorvo DWM3000 UWB** | [Qorvo resmî ürün sayfası](https://www.qorvo.com/products/p/DWM3000) | DW3110 tabanlı UWB modülü; SPI, IRQ/reset bağlantıları, anten ve modül revizyonu doğrulanmalı. ARES deposunda doğrulanmış sürücü henüz seçilmiş sayılmıyor. İki düğümlü ranging testi, kalibrasyon ve yerel mevzuat kontrolü gerekir. |
| **XMOS XVF3800 + 4 × PDM MEMS mikrofon** | [XMOS XVF3800 ürün/doküman sayfası](https://www.xmos.com/xvf3800/) · [Datasheet PDF](https://www.xmos.com/download/XVF3800-Documentation%28v1_0_0%29.pdf) · [Resmî firmware ve araçlar](https://github.com/respeaker/reSpeaker_XVF3800_USB_4MIC_ARRAY) | Mikrofon saati, dizi geometrisi, firmware varyantı ve host arayüzü kritik. USB sürümü USB host gerektirir; DENEYAP Kart V2'nin USB host gibi kullanılabileceği varsayılmamalı. I²S kullanımı için firmware, pinout ve clock/master-slave modu doğrulanmalı. Konuşma odaklı beamforming özellikleri, enkaz altındaki zayıf sesleri algılama performansını kendiliğinden garanti etmez. |
| **Geospace GS-One LF geophone** | [Geospace resmî GS-One LF sayfası](https://www.geospace.com/products/sensors-and-geophones/gs-one-lf/) | 4.5 Hz ve 5 Hz varyantları bulunur; sipariş edilen varyant teyit edilmeli. Ham bobin sinyali için düşük gürültülü analog ön uç, kazanç/filtre, ADC aralığı, mekanik montaj ve kalibrasyon gerekir. Kütle ve boyutlar hafif UAV tasarımında önemli yük oluşturabilir. |
| **Garmin LIDAR-Lite v3** | [Garmin kullanım kılavuzu ve teknik özellikler PDF](https://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf) · [Garmin ürün sayfası](https://www.garmin.com.tr/lidar-lite-v3) | Üretici dokümanı I²C/PWM arayüzünü ve lazer güvenlik uyarılarını açıklar. Güç rayı, lojik seviyeleri, yüzey/ışık etkileri, gerçek mesafe doğruluğu ve uçuş titreşimiyle ölçüm davranışı test edilmeli. Optik/lazer muhafazası değiştirilmemeli. |

### 2.3 İtki, güç ve mekanik malzemeler

| Parça / malzeme | Birincil kaynak | Teknik not / açık doğrulama |
|---|---|---|
| **T-Motor U8 Lite KV150** | [T-Motor resmî U8 Lite ürün sayfası](https://store.tmotor.com/product/u8-lite-u-efficiency-kv150.html) · [T-Motor U-serisi kaynakları](https://uav-en.tmotor.com/2024/V-Series_1114/1253.html) | **Model uyuşmazlığı riski:** erişilen ürün sayfası U8 Lite için KV85 değerini gösterirken başka resmî ürün tablolarında U8 Lite KV150 adı geçiyor. Bu nedenle KV150 motorun tam ürün kodu/etiketi ve ona ait thrust table doğrulanmadan voltaj, pervane, akım, itki veya batarya değeri kesinleştirilmemeli. |
| **Pololu D24V50F5** | [Pololu teknik özellikler](https://www.pololu.com/product/2851/specs) · [Ürün açıklaması](https://www.pololu.com/product/2851) | 5 V sabit çıkışlı buck regülatör; üretici 6–38 V giriş ve tipik olarak 5 A'ya kadar sürekli çıkış belirtir. Gerçek sürekli akım termal koşullara bağlıdır. Bu regülatör motor sürücüsü/ESC beslemesi değildir; sensör ve lojik yükleri için toplam akım, transient ve ısınma testleri gerekir. |
| **Karbon fiber kumaş** | [WEST SYSTEM kompozit ve epoksi teknik kaynakları](https://www.westsystem.com/) | Kumaşın dokuma tipi, areal ağırlığı, reçine sistemi ve katman yönleri belirtilmemiş. Laminasyon tasarımı; fiber yönlenmesi, reçine oranı, kür koşulu, katman kalınlığı ve bağlantı noktalarıyla birlikte doğrulanmalı. |
| **Sert karton** | Bu envanterde üretici/grade belirtilmedi | Prototip geometrisi ve elektronik yerleşim maketi için kullanılabilir; uçuşa elverişli taşıyıcı yapı veya rotor muhafazası olarak doğrulanmış değildir. Nem, yorulma, darbe ve titreşim dayanımı için veri yok. |
| **WEST SYSTEM G/flex 655** | [G/flex teknik veri sayfası PDF](https://www.westsystem.com/app/uploads/2023/05/655-G-flexTechnical-Data-Sheet.pdf) · [Güvenlik bilgi formları](https://www.westsystem.com/safety/safety-data-sheets/) | Toklaştırılmış, koyulaştırılmış iki bileşenli epoksi yapıştırıcıdır; üretici veri sayfası karışım ve çalışma sürelerini açıklar. **Yapısal yapıştırma ile kumaş laminasyonu/kaplama aynı işlem değildir.** Yüzey hazırlığı, doğru oran, kür, sıcaklık, numune bağlantı testi ve SDS/PPE şartları uygulanmalı. |
| **Batarya, ESC, pervane, uçuş kontrolcüsü, güç dağıtımı** | Henüz kesin üretici/parça numarası yok | Mevcut kayıt bu parçaları tamamlanmış bir alt sistem olarak tanımlamaz. Batarya hücre sayısı/akımı, ESC akım sınırı, üreticinin motor-pervane-itki tablosu, toplam kalkış kütlesi, kablolama, sigorta ve uçuş kontrolcüsü/failsafe doğrulanmadan uçuşa hazır kabul edilmemeli. |

---

## 3. Yazılım, kütüphaneler ve altyapı kaynakları

### 3.1 Mevcut depo kayıtlarında belirtilen bağımlılıklar

| Bileşen | Kaynak | Durum / not |
|---|---|---|
| FLIR Lepton Arduino driver | [NachtRaveVL/Lepton-FLiR-Arduino](https://github.com/NachtRaveVL/Lepton-FLiR-Arduino) | [sensor-libraries.md](docs/hardware/sensor-libraries.md), sürümü 2.1.1 ve upstream commit 8577d336ecfc12dc8f3d0612cf8b6dcd681d626f olarak kaydediyor. Gerçek breakout üzerinde çalıştığı ayrıca doğrulanmalı. |
| Garmin LIDAR-Lite Arduino library | [PlatformIO Registry](https://registry.platformio.org/libraries/garmin/LIDAR-Lite) | Proje dokümanında 3.0.6 olarak belirtilmiş. Kütüphanenin derlenmesi gerçek sensör ölçümünün çalıştığını kanıtlamaz. |
| DENEYAP Kart V2 firmware framework | [Arduino-ESP32](https://github.com/espressif/arduino-esp32) · [Espressif dokümantasyonu](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/) | Kartın tam revizyonu ve seçilen PlatformIO board tanımı eşleştirilmeli. Resmî ürün sayfası ESP32-S3 belirtirken mevcut sensor-libraries.md metnindeki “classic ESP32” ifadesi yeniden kontrol edilmeli. |
| SD kart API | [Arduino-ESP32 SD](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD) · [SDMMC API](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD_MMC) | SD-SPI ve SDMMC farklı arayüzlerdir. Kart üzerindeki okuyucu/pinout doğrulanmadan API veya pin seçimi kesinleştirilmemeli. |
| C++ build | [CMake resmî dokümantasyonu](https://cmake.org/documentation/) | ARES ai_core/CMakeLists.txt C++20 ve CMake 3.20 minimum sürümünü tanımlar. |
| Go | [Go resmî dokümantasyonu](https://go.dev/doc/) | bridge_service içindeki go.mod ve CI yapılandırması gerçek sürüm kaynağıdır. |
| MQTT broker | [Eclipse Mosquitto dokümantasyonu](https://mosquitto.org/documentation/) | ARES güvenlik dokümanı TLS, anonymous erişimin kapatılması, kimlik başına ACL ve güvenli provisioning gerektirir. Bunlar tasarım şartlarıdır; dokümanda bulunmaları tamamlanmış uygulama anlamına gelmez. |
| MQTT protokolü | [OASIS MQTT standardı](https://mqtt.org/mqtt-specification/) | Mesaj sözleşmesi, QoS/retained davranışı, oturum ve komut tekrar oynatma politikası uygulamada tanımlanmalı ve test edilmeli. |
| Yerel LLM runtime | [llama.cpp](https://github.com/ggml-org/llama.cpp) | Gemma GGUF yürütme adayı. Host RAM, inference hızı, model lisansı ve quantization uyumluluğu gerçek hedef bilgisayarda ölçülmeli. |
| Gemma 4 | [Google Gemma 4 model card](https://ai.google.dev/gemma/docs/core/model_card_4) · [Google Gemma dokümantasyonu](https://ai.google.dev/gemma/docs) | Modelin giriş/çıkış yetenekleri ve lisans bilgisi doğrudan model card/şartlardan kontrol edilmeli. AI çıktısı yalnızca yardımcı kanıt yorumudur; uçuş kontrolü değildir. |
| Astro web framework | [Astro resmî dokümantasyonu](https://docs.astro.build/) | ARES sitesinin mevcut site/ dosyaları ve package manifesti esas alınmalı; framework adı tek başına uygulanmış özellik kanıtı değildir. |
| Flutter | [Flutter resmî dokümantasyonu](https://docs.flutter.dev/) | README'de mobil uygulama hedefi olarak anılır; uçtan uca uygulama tamamlandı varsayılmamalı. |
| Bluetooth / BLE | [Bluetooth SIG — teknik kaynaklar](https://www.bluetooth.com/specifications/) | İlk kayıt için kesin kart revizyonu, eşleştirme modu, cihaz kimlik doğrulaması ve challenge-response tasarımı gerekir. BLE cihaz adı veya PIN tek başına kimlik kanıtı değildir. |

### 3.2 Kütüphane seçiminde açık uyarılar

- **Qorvo DWM3000:** ARES dokümanında doğrulanmış uyumlu PlatformIO paketi henüz seçilmemiştir. Yalnızca DW1000 için yazılmış bir sürücüyü uyumlu kabul etmeyin.
- **XMOS XVF3800:** USB ve I²S firmware varyantları farklı host/arayüz gereksinimlerine sahiptir. DENEYAP Kart V2 USB host varsayımı yapılmamalı.
- **Geospace GS-One LF:** Analog geophone; dijital sensör sürücüsü tek başına yeterli değildir. Analog ön uç, ADC ve kalibrasyon tasarımı gerektirir.
- **74LVC2G17:** Donanım lojik buffer'ıdır; yazılım kütüphanesi değildir ve genel amaçlı çift yönlü seviye dönüştürücü yerine kullanılamaz.
- **Lepton ve LIDAR-Lite:** CI derlemesinin başarılı olması, gerçek donanım iletişimi, radyometri veya mesafe doğruluğunun kanıtı değildir.

---

## 4. Güvenlik, haberleşme ve uçuş emniyeti kaynakları

| Konu | Kaynak | Uygulanacak ilke |
|---|---|---|
| MQTT broker ve kimlikler | [Mosquitto](https://mosquitto.org/documentation/) · [ARES MQTT pairing tasarımı](docs/security/mqtt-pairing.md) | Cihaz başına benzersiz kimlik/secret, TLS sertifika doğrulaması, en az yetkili ACL, revoke/rotate ve loglarda secret tutmama. |
| Bluetooth ilk kayıt | [Bluetooth SIG](https://www.bluetooth.com/specifications/) · [ARES Bluetooth-first tasarımı](docs/security/bluetooth-first-setup.md) | Tam kart revizyonu ve BLE yeteneği doğrulanmadan uygulama başlatılmamalı. Fiziksel varlık, kimlik doğrulaması, şifreli kanal ve replay koruması gerekir. |
| Uçuş emniyeti | [ARES mimarisi](docs/architecture.md) | Gemma, Windows, telefon veya MQTT motor PWM, uçuş stabilizasyonu ya da donanımsal failsafe üzerinde doğrudan yetkili olmamalı. Bağlantı kaybında doğrulanmış uçuş kontrolcüsü kendi failsafe davranışını sürdürmeli. |
| Model sorumluluğu | [Gemma 4 model card](https://ai.google.dev/gemma/docs/core/model_card_4) | Model çıktısı olasılıksal tavsiyedir. İnsan tespiti iddiası deneysel veri ve hata oranı ölçümü olmadan yapılmamalı. |
| Lisanslar | [ARES LICENSE](LICENSE) · [LICENSES.md](LICENSES.md) · [Google Gemma model card](https://ai.google.dev/gemma/docs/core/model_card_4) | Proje kodu, üçüncü taraf kütüphaneler ve model ağırlıkları farklı lisanslara tabi olabilir; dağıtımdan önce her birinin lisansı/NOTICE şartı kontrol edilmeli. |

---

## 5. Test ve doğrulama için gereken kanıtlar

Her bileşen için test kaydı aşağıdaki bilgileri içermelidir:

- Üretici, tam parça numarası, revizyon ve seri/lot bilgisi (gizli veya hassas seri numaraları halka açık dosyaya yazılmadan).
- Kullanılan veri sayfasının sürümü/tarihi ve ilgili sayfa/tablo.
- Kart revizyonu, kablo, pinout, güç kaynağı ve ölçüm cihazları.
- Firmware/driver sürümü ve commit SHA.
- Ortam koşulları, örnek sayısı, kalibrasyon referansı ve ham test çıktısı.
- Beklenen değer, ölçülen değer, tolerans, hata oranı ve kabul/red kararı.
- Testin yalnızca derleme, masaüstü simülasyonu, tezgâh testi veya uçuş testi olduğu açıkça yazılmalı.

Önerilen minimum doğrulama:
1. Kartın MCU, pinout, radyo/BLE ve güç özelliklerini fiziksel kartla eşleştir.
2. Her sensörü ayrı ayrı bilinen referanslarla tezgâhta doğrula.
3. SD yazma hızını, uzun süreli kaydı ve güç kesintisi/bozuk kayıt davranışını test et.
4. Sensörler arası zaman damgası hizalaması, paket kaybı, sıra numarası ve hatalı veri davranışını doğrula.
5. Motor çalışırken titreşim, EMI, besleme çökmesi ve termal davranışı ölç.
6. İtki sistemi için tam motor + ESC + pervane + batarya + koruma + toplam kütle test planı hazırla.
7. Enkaz benzeri kontrollü senaryolarda termal/akustik/titreşim algılamasını kör testlerle değerlendir; yanlış pozitif ve yanlış negatif oranlarını raporla.
8. Uçuş testlerinden önce bağımsız uçuş kontrolcüsü, failsafe, mekanik güvenlik ve yerel mevzuat gereksinimlerini tamamla.

---

## 6. Açık kalan araştırma maddeleri

- [ ] DENEYAP Kart V2'nin fiziksel revizyonu ve platformio.ini board tanımı eşleştirilsin; ESP32-S3 bilgisiyle dokümanlardaki “classic ESP32” ifadesi uzlaştırılsın.
- [ ] DWM3000 modül revizyonu, resmî datasheet revizyonu, uygun lisanslı sürücü ve iki düğümlü test düzeneği kaydedilsin.
- [ ] XMOS XVF3800 kart/firmware varyantı (USB veya I²S), host bağlantısı ve güç gereksinimleri teyit edilsin.
- [ ] Geospace GS-One LF'nin 4.5 Hz / 5 Hz varyantı ve montaj yönü belirlenip analog ön uç tasarlansın.
- [ ] FLIR Lepton breakout kartı ve radiometric yapılandırması kesinleştirilsin.
- [ ] U8 Lite **KV150** motorun tam ürün kodu ve doğru üretici thrust table'ı bulunup KV85 sayfasından ayrıştırılsın.
- [ ] ESC, pervane, batarya, uçuş kontrolcüsü, güç dağıtımı, sigorta, kablo ve konektörler için parça numaralı BOM tamamlansın.
- [ ] 32 GB SD kartın marka/modeli, sınıfı ve gerçek yazma performansı kaydedilsin.
- [ ] Karbon fiber kumaşın türü/gramajı ve yapısal reçine sistemi; kartonun yalnızca prototip amaçlı mı olduğu belirtilecek şekilde dokümante edilsin.
- [ ] Her kaynağın erişim tarihi, kullanılan revizyonu ve kaynakla ilişkili test kanıtı eklensin.

## 7. Kaynakların güncellenmesi

Yeni bir ürün, kütüphane, model, malzeme veya mimari karar araştırıldığında bu dosyaya şu formatla ekleyin:

- **Ad / tam model:**
- **Üretici / proje:**
- **Birincil kaynak URL:**
- **Doküman adı, revizyonu ve tarihi:**
- **Hangi karar için kullanıldı:**
- **Önemli teknik sınırlar / uyumluluk notları:**
- **Durum:** aday / seçildi / tezgâhta test edildi / bütünleşik test edildi
- **Test kaydı veya commit:**
- **Son kontrol tarihi:**

Kaynakça, doğrulanmamış bir parçayı kesin seçilmiş malzemeye dönüştürmez; her iddia, ilgili üretici belgesi ve test kanıtıyla desteklenmelidir.
