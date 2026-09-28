# ARES — Deneyap Osmaniye
Drone destekli enkaz arama ve kurtarma araştırma/prototip projesi.

Bu depo doğrulanmış donanım kararlarını, firmware'i, haberleşme protokolünü, testleri ve proje sitesini birlikte tutar. ARES prototiptir; gerçek operasyonlarda profesyonel ekiplerin ve sertifikalı ekipmanların yerine geçmez.

## Başlangıç
- [Mimari ve yol haritası](docs/architecture.md)
- [Araştırma/karar günlüğü](docs/research/decision-log.md)
- [UART eş-düğüm protokolü](docs/protocol/uart-peer.md)
- [Donanım envanteri](docs/hardware/bom.md)
- [Test ve güvenlik planı](docs/validation/test-plan.md)
- [Proje sitesi](site/index.html)

## Mevcut durum
İki Deneyap Kart V2 ve UART tabanlı eşit düğümlü haberleşme hedefleniyor. Sensörler ve güç bileşenleri doğrulama aşamasında; kesin parça seçimi üretici dokümanları, elektriksel uyumluluk, ağırlık/güç bütçesi ve test sonuçlarıyla yapılacak.

## Site
GitHub Actions Pages iş akışı eklendi. Depo private olduğundan Pages erişilebilirliği GitHub planı/ayarlarına bağlıdır; Settings → Pages bölümünden GitHub Actions kaynağını etkinleştirin.
