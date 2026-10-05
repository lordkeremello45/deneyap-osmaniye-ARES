# ARES Drone Telemetry

Drone üzerindeki FLIR Lepton 3.5, XMOS XVF3800, Geospace GS-one LF, Qorvo DWM3000 ve Garmin Lidar-Lite V3 kaynaklarından sürekli gelen verinin giriş katmanıdır.

## Veri akışı

Sensors -> acquisition -> packet -> validation -> recorder -> fusion_input

Bu katman AI Core'dan bağımsızdır. AI kapalı olsa bile ham telemetry alınabilir, doğrulanabilir ve kaydedilebilir.

## Sorumluluklar

- sürekli sensör örnekleme verisini kabul etmek
- sequence ve timestamp kontrolü
- CRC ile paket bütünlüğü
- sensör kalite/durum bilgisi
- ham veriyi replay için saklama
- AI Core'a yalnızca doğrulanmış özellikleri aktarma

AI Core uçuş kontrolü yapmaz.
