# ARES Firebase otomasyonu

Firebase, ARES'in bulut otomasyon katmanıdır; Go Bridge ve C++ AI Core'un yerine geçmez.

## Veri akışı

Deneyap sensörleri -> Go Bridge -> C++ AI Core -> yapılandırılmış sonuç -> Go Bridge -> Firebase/Firestore -> Flutter Android/iOS

## Servisler

- Firebase Authentication: operatör kimliği ve oturum.
- Cloud Firestore: cihaz durumu, yapılandırılmış AI olayları ve denetim kayıtları.
- Firebase Cloud Messaging: doğrulanmış insan varlığı adayları ve kritik sistem olayları için operatör bildirimi.
- Firebase App Check: üretim mobil API koruması.

## Kurallar

- Ham yüksek hızlı sensör akışları varsayılan olarak yerelde/bridge üzerinde tutulur.
- Firestore'a hız sınırlandırılmış yapılandırılmış telemetri ve AI sonuçları gönderilir.
- AI doğrudan uçuş veya kurtarma aktüatörlerini kontrol etmez.
- Firebase servis hesabı anahtarları GitHub'a commit edilmez.
- CI, Firebase işlemleri için GitHub Actions secrets kullanır.

## Kurulum

Firebase projesi oluşturulmalı ve Android/iOS uygulamaları kaydedilmelidir. FlutterFire CLI ile üretilen firebase_options.dart güvenli şekilde projeye eklenmelidir. Servis hesabı JSON dosyası commit edilmemelidir.

## Otomasyon hedefleri

1. GitHub Actions Flutter build/analyze/test çalıştırır.
2. Firebase Authentication operatör erişimini yönetir.
3. Firestore yapılandırılmış telemetri ve AI sonuçlarını saklar.
4. FCM doğrulanmış uyarıları mobil operatöre iletir.
5. GitHub Actions gerekli Firebase kontrol/deploy adımlarını repository secrets ile çalıştırabilir.
