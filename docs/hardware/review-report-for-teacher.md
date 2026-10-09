# ARES Donanım İnceleme Raporu

**Hedef kitle:** Donanım sorumlusu / öğretmen incelemesi  
**Tarih:** 10 Ekim 2026  
**Durum:** Ön inceleme; uçuşa uygunluk onayı değildir  
**Kaynak ana dosya:** [source.md](../../source.md)  
**BOM:** [bom.md](bom.md) · [İtki sistemi kararı](propulsion-selection.md) · [Sensör sürücüleri](sensor-libraries.md)

## 1. Yönetici özeti

ARES'in mevcut malzeme listesi tek başına uçuşa hazır bir UAV sistemi oluşturmuyor. Özellikle motor/ESC/pervane/batarya eşleşmesi, uçuş kontrolcüsü, güç dağıtımı, toplam kütle ve sensör arayüzleri fiziksel parça numaralarıyla kesinleştirilmeden motorlu uçuşa geçilmemeli.

Aşağıdaki maddeler satın alma veya bağlantı yapmadan önce kapatılmalıdır. Bu rapor donanım kararlarını öğretmen ve donanım ekibinin incelemesi için toplar; yazılım ekibi fiziksel pinleri veya motor konfigürasyonunu varsaymamalıdır.

## 2. Kritik kararlar

| Öncelik | Konu | Bulgu | Gerekli karar / kanıt |
|---|---|---|---|
| P0 — Uçuş engeli | T-Motor U8 Lite KV150 | Üretici sayfasında KV85/KV150 ayrı varyantlar. Ürün sayfasındaki KV150 6S/12S tabloları ile V-Link eşleştirme tablosundaki 13S/MF24 bilgisi uyuşmuyor. | Motor etiketi/SKU fotoğrafı ve T-Motor'dan yazılı hücre sayısı + ESC + pervane eşleşmesi. |
| P0 — Uçuş engeli | Uçuş kontrolcüsü | Mevcut yazılımda gerçek flight-controller entegrasyonu yok; DENEYAP kartı uçuş kontrolcüsü varsayılmıyor. | Rotor düzeni ve gereksinimlere göre ayrı uçuş kontrolcüsü, firmware ve failsafe test planı seçilsin. |
| P0 — Uçuş engeli | Batarya / ESC / pervane | Kesin parça numaraları, gerilim, sürekli akım, konektör ve pervane eşleşmesi yok. | Motor üreticisinin onayladığı tek bir güç sistemi konfigürasyonu ve veri sayfaları. |
| P0 — Uçuş engeli | Ağırlık ve itki bütçesi | MTOW, rotor sayısı, yük kütlesi ve hedef havada kalış süresi belirlenmemiş. | Kütle tablosu, hover thrust/motor hesabı, güvenlik marjı ve test düzeneği. |
| P1 — Sensör entegrasyonu | DENEYAP Kart V2 | Depodaki PlatformIO hedefi fiziksel kart revizyonu ve pinout ile henüz doğrulanmış değil. | Kart üzerindeki MCU/revizyon, pinout, besleme ve I/O seviyeleri kaydedilsin. |
| P1 — UWB | Qorvo DWM3000 | SPI/IRQ/RESET pinleri, modül revizyonu ve sürücü portu donanımda doğrulanmadı. | Şema/pin eşlemesi, anten/yerleşim kontrolü ve iki düğümlü mesafe testi. |
| P1 — Akustik | XMOS XVF3800 + 4 mikrofon | Hazır USB/UA kartı mı yoksa çıplak çip ve mikrofonlar mı olduğu belli değil. Bu iki durum farklı donanım gerektirir. | Kart/ürün kodu, firmware varyantı, güç gereksinimi, USB veya I²S arayüzü doğrulansın. |
| P1 — Sismik | Geospace GS-One LF | Analog geophone sinyali doğrudan MCU girişine bağlanamaz; uygun analog ön uç ve ADC aralığı gerekir. | Sensör varyantı, hassasiyet, düşük gürültülü kazanç/filtre devresi ve kalibrasyon. |
| P1 — Termal | FLIR Lepton 3.5 | Breakout, VoSPI/CCI bağlantısı, besleme, FOV ve radyometri ayarları belli değil. | Tam breakout modeli, güç/logic seviyeleri, SPI/I²C bağlantısı ve referans sıcaklık testi. |
| P1 — Mesafe | Garmin LIDAR-Lite v3 | Besleme, I²C/UART arayüz seçimi, seviye uyumu ve kablo yerleşimi doğrulanmalı. | Kesin varyant, bağlantı şeması ve bilinen mesafelerde ölçüm testi. |
| P2 — Güç | Pololu D24V50F5 | 5 V buck regülatördür; motor beslemesi veya ESC yerine kullanılamaz. | Sensör/lojik yüklerinin tepe akımı ve termal koşullarıyla ayrı 5 V hattı tasarlansın. |
| P2 — Yapı | Karbon elyafı, rijit karton, G/flex 655 | Karton prototip/yerleşim maketi içindir; uçuşa uygun yapısal malzeme olarak kabul edilmemeli. Epoksi laminasyon ve yapısal yapıştırma farklı işlemlerdir. | Gövde tasarımı, bağlantı noktaları, titreşim, kütle, koruma ve yapıştırma/laminasyon prosedürü doğrulansın. |

## 3. Motor ve itki sistemi: özel dikkat

Üretici kaynakları:
- [T-Motor U8 Lite KV150 ürün sayfası](https://store.tmotor.com/product/u8-lite-u-efficiency-kv150.html)
- [T-Motor V-Link eşleştirme tablosu](https://store.tmotor.com/product/v-link.html)

Ürün sayfasında KV85 varsayılan teknik tablo olarak gösterilebildiği için bu tabloyu KV150 verisi kabul etmek hatalı olur. KV150 bölümünde ayrı kombinasyonlar bulunur; V-Link tablosunda ayrıca 13S/MF24 eşleştirmesi listelenmesi konfigürasyon kararını belirsizleştirir.

**İstenen işlem:** Motor etiketi ve satın alınacak tam SKU üzerinden T-Motor'dan yazılı doğrulama alın. Doğrulama gelene kadar batarya hücre sayısı, ESC, pervane ve maksimum akım kesin BOM olarak onaylanmamalıdır.

Propulsion-selection belgesindeki G28×9.2 + Alpha 60A 6S + 6S LiPo kombinasyonu yalnızca üretici sayfasında görülen geçici statik test adayıdır; uçuş onayı değildir. Pervaneli testler uygun itki standı, sabit fikstür, uzaktan acil kesme, fiziksel güvenlik alanı ve koruyucu ekipman olmadan yapılmamalıdır.

## 4. Güç mimarisi için gerekli çıktılar

Donanım ekibi şu tabloyu gerçek parça numaraları ve ölçümlerle doldurmalı:

| Hat / bileşen | Gerekli veri |
|---|---|
| Batarya | Kimya, hücre sayısı, kapasite, sürekli/tepe deşarj, kütle, konektör |
| ESC ve motor | Tam model, gerilim aralığı, sürekli/tepe akım ve soğutma koşulu |
| Pervane | Üretici parça numarası, çap/adım, göbek adaptörü, izin verilen devir |
| Güç dağıtımı | Hat başına akım, sigorta, kablo kesiti, konektör ve anti-spark ihtiyacı |
| 5 V / 3.3 V lojik | Sensörlerin çalışma/tepe akımları, regülatör sıcaklığı, gürültü |
| Kütle | Gövde, motorlar, ESC'ler, batarya, koruma, kablo ve sensör yüklerinin toplamı |
| Uçuş kontrolcüsü | Rotor geometrisi, aktüatör çıkışları, RC/telemetri, GNSS ihtiyacı ve failsafe |

## 5. Donanım doğrulama sırası

1. Tüm parça etiketlerini, revizyonları ve üretici veri sayfalarını kaydet.
2. Motor konfigürasyonu için yazılı üretici teyidi al.
3. Toplam kütle ve itki bütçesini hesapla; pervane koruması ve mekanik boşlukları dahil et.
4. Güç raylarını sigorta, kablo, konektör, akım ve termal marjlarla tasarla.
5. Sensörleri tek tek, motorlardan ayrı beslemeyle tezgâhta çalıştır; veri sayfası arayüzleri ve mantık seviyelerini doğrula.
6. Sensör başına bilinen referansla kalibrasyon yap; sıcaklık, mesafe, ses ve titreşim ölçümlerini kaydet.
7. Motor/ESC testini önce pervanesiz, sonra uygun sabit test düzeneğinde ve onaylı pervaneyle yap.
8. EMI/titreşim testlerinde UART/SPI/I²C hataları, sensör kayması, sıcaklık ve gerilim çökmesini kaydet.
9. Uçuş kontrolcüsünün RC/link kaybı, düşük batarya, sensör arızası ve acil durdurma davranışlarını bağımsız doğrula.
10. Tüm ölçüm raporları ve risk kontrolleri imzalanmadan serbest uçuşa geçme.

## 6. Donanım sorumlusu için teslim listesi

- [ ] DENEYAP Kart V2'nin gerçek kart revizyonu ve pinout fotoğrafı
- [ ] U8 Lite KV150 motor etiketi/SKU ve üretici yazılı eşleştirme cevabı
- [ ] ESC, pervane, batarya ve güç dağıtımı parça numaralı BOM
- [ ] Uçuş kontrolcüsü ve failsafe test planı
- [ ] Tüm sistemin kütle/itki/endurance hesapları
- [ ] Lepton, DWM3000, XVF3800, GS-One LF ve LIDAR-Lite arayüz/kalibrasyon kayıtları
- [ ] Güç, termal, titreşim, EMI ve kablo sabitleme test sonuçları
- [ ] Açık kalan riskler ve uçuşa izin veren resmi inceleme kaydı

## 7. Yazılım ekibi için arayüz notu

Firmware tarafında pin atamaları ve gerçek sensör sürücüleri fiziksel revizyonlar teyit edilene kadar kesinleştirilmeyecek. Yazılımın sensör değerlerini alması, tek başına ölçümlerin kalibre olduğu veya enkaz altında insan tespitinin güvenilir olduğu anlamına gelmez. Gemma/AI Core yalnızca operatöre yardımcı sonuç üretir; uçuş kontrolü ve failsafe ayrı uçuş kontrolcüsünde kalır.
