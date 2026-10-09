import 'package:flutter_test/flutter_test.dart';
import 'package:ares_mobile/main.dart';

void main() {
  testWidgets('ARES dashboard renders its main sensor panels', (tester) async {
    await tester.pumpWidget(const AresApp());

    expect(find.text('ARES — Arama Sistemi'), findsOneWidget);
    expect(find.text('Termal'), findsOneWidget);
    expect(find.text('UWB'), findsOneWidget);
    expect(find.text('Akustik / Sismik'), findsOneWidget);
    expect(find.text('LiDAR'), findsOneWidget);
    expect(find.text('Telemetriyi yenile'), findsOneWidget);
  });
}
