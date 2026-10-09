import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

const String _bridgeUrl = String.fromEnvironment('ARES_BRIDGE_URL');

void main() => runApp(const AresApp());

class AresApp extends StatelessWidget {
  const AresApp({super.key});

  @override
  Widget build(BuildContext context) => MaterialApp(
        debugShowCheckedModeBanner: false,
        title: 'ARES',
        theme: ThemeData.dark(useMaterial3: true),
        home: const Dashboard(),
      );
}

class Dashboard extends StatefulWidget {
  const Dashboard({super.key});

  @override
  State<Dashboard> createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  Map<String, dynamic> telemetry = const {
    'status': 'Bağlantı adresi yapılandırılmadı',
  };
  bool loading = false;

  Future<void> refresh() async {
    final uri = Uri.tryParse(_bridgeUrl);
    if (uri == null ||
        !uri.hasAuthority ||
        (uri.scheme != 'https' &&
            !(uri.scheme == 'http' &&
                (uri.host == '127.0.0.1' ||
                    uri.host == 'localhost' ||
                    uri.host == '10.0.2.2'))) {
      setState(() {
        telemetry = const {
          'status': 'Geçersiz adres; uzaktan bağlantı için HTTPS gerekli',
        };
      });
      return;
    }

    setState(() => loading = true);
    try {
      final response = await http
          .get(uri.replace(path: '/api/v1/telemetry'))
          .timeout(const Duration(seconds: 3));
      final body = jsonDecode(response.body) as Map<String, dynamic>;
      if (!mounted) return;
      if (response.statusCode == 200 || response.statusCode == 503) {
        setState(() => telemetry = body);
      } else {
        setState(() {
          telemetry = {
            'status': 'Bridge HTTP ${response.statusCode}',
          };
        });
      }
    } on TimeoutException {
      if (mounted) {
        setState(() => telemetry = const {'status': 'Bridge zaman aşımı'});
      }
    } catch (_) {
      if (mounted) {
        setState(() => telemetry = const {'status': 'Bridge bağlantısı yok'});
      }
    } finally {
      if (mounted) setState(() => loading = false);
    }
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(title: const Text('ARES — Arama Sistemi')),
        body: ListView(
          padding: const EdgeInsets.all(16),
          children: [
            Card(
              child: ListTile(
                title: const Text('Sistem'),
                subtitle: Text('${telemetry['status']}'),
              ),
            ),
            const Card(
              child: ListTile(
                title: Text('Termal'),
                subtitle: Text('Bekleniyor'),
              ),
            ),
            const Card(
              child: ListTile(
                title: Text('UWB'),
                subtitle: Text('Bekleniyor'),
              ),
            ),
            const Card(
              child: ListTile(
                title: Text('Akustik / Sismik'),
                subtitle: Text('Bekleniyor'),
              ),
            ),
            const Card(
              child: ListTile(
                title: Text('LiDAR'),
                subtitle: Text('Bekleniyor'),
              ),
            ),
            const SizedBox(height: 12),
            FilledButton.icon(
              onPressed: loading || _bridgeUrl.isEmpty ? null : refresh,
              icon: const Icon(Icons.sync),
              label: const Text('Telemetriyi yenile'),
            ),
            if (_bridgeUrl.isEmpty)
              const Padding(
                padding: EdgeInsets.only(top: 8),
                child: Text(
                  'Telemetri bağlantısı henüz etkin değil. Geliştirme için '
                  'ARES_BRIDGE_URL tanımlanmalı; telefondan uzaktan erişim '
                  'için kimlik doğrulamalı HTTPS/MQTT entegrasyonu gerekir.',
                ),
              ),
          ],
        ),
      );
}
