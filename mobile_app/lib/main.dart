import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

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
  @override State<Dashboard> createState() => _DashboardState();
}

class _DashboardState extends State<Dashboard> {
  Map<String, dynamic> telemetry = const {'status': 'Bağlantı bekleniyor'};
  bool loading = false;

  Future<void> refresh() async {
    setState(() => loading = true);
    try {
      final r = await http.get(Uri.parse('http://127.0.0.1:8080/api/v1/telemetry'));
      setState(() => telemetry = jsonDecode(r.body) as Map<String, dynamic>);
    } catch (_) {
      setState(() => telemetry = const {'status': 'Bridge bağlantısı yok'});
    } finally { setState(() => loading = false); }
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(title: const Text('ARES — Arama Sistemi')),
    body: ListView(padding: const EdgeInsets.all(16), children: [
      Card(child: ListTile(title: const Text('Sistem'), subtitle: Text('${telemetry['status']}'))),
      const Card(child: ListTile(title: Text('Termal'), subtitle: Text('Bekleniyor'))),
      const Card(child: ListTile(title: Text('UWB'), subtitle: Text('Bekleniyor'))),
      const Card(child: ListTile(title: Text('Akustik / Sismik'), subtitle: Text('Bekleniyor'))),
      const Card(child: ListTile(title: Text('LiDAR'), subtitle: Text('Bekleniyor'))),
      const SizedBox(height: 12),
      FilledButton.icon(onPressed: loading ? null : refresh, icon: const Icon(Icons.sync), label: const Text('Telemetriyi yenile')),
    ]),
  );
}
