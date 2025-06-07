// This is a basic Flutter widget test.
//
// To perform an interaction with a widget in your test, use the WidgetTester
// utility in the flutter_test package. For example, you can send tap and scroll
// gestures. You can also use WidgetTester to find child widgets in the widget
// tree, read text, and verify that the values of widget properties are correct.

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:ui/main.dart';
import 'package:ui/graph_editor.dart';
import 'package:ui/objects.dart';

void main() {
  late ByteData mockXmlData;

  setUpAll(() {
    // Create mock XML data once
    final mockXml = '''
<?xml version="1.0" encoding="UTF-8"?>
<Root>
  <Target name="TestTarget">
    <Kernel name="TestKernel">
      <Inputs>
        <Input>input1</Input>
      </Inputs>
      <Outputs>
        <Output>output1</Output>
      </Outputs>
    </Kernel>
  </Target>
</Root>
''';
    final mockXmlBytes = Uint8List.fromList(mockXml.codeUnits);
    mockXmlData = ByteData.view(mockXmlBytes.buffer);

    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMessageHandler('flutter/assets', (ByteData? message) async {
      if (message == null) return null;
      final String assetPath = String.fromCharCodes(message.buffer.asUint8List());
      if (assetPath == 'assets/supported.xml') {
        return mockXmlData;
      }
      return null;
    });
  });

  testWidgets('App functionality test', (WidgetTester tester) async {
    // Launch app and wait for initial build
    await tester.pumpWidget(const GraphEditorApp());
    await tester.pump();

    // Verify initial UI
    expect(find.text('Edge Studio'), findsOneWidget);
    expect(find.byIcon(Icons.code_rounded), findsOneWidget);
    expect(find.byType(GraphEditor), findsOneWidget);
    expect(find.byType(GraphListPanel), findsOneWidget);

    // Wait for XML to load
    final state = tester.state<GraphEditorState>(find.byType(GraphEditor));
    await state.xmlLoaded;
    await tester.pump();

    // Test graph operations
    await tester.tap(find.byType(IconButton).first);
    await tester.pump();

    await tester.tap(find.byType(Chip).first);
    await tester.pump();

    await tester.sendKeyEvent(LogicalKeyboardKey.delete);
    await tester.pump();

    expect(find.byType(GraphEditor), findsOneWidget);

    // Test node addition
    await tester.tap(find.byType(IconButton).first);
    await tester.pump();

    final graphArea = find.byType(CustomPaint).first;
    await tester.tapAt(tester.getCenter(graphArea));
    await tester.pump();

    expect(find.byType(CustomPaint), findsWidgets);

    // Test export menu
    await tester.tap(find.byIcon(Icons.code_rounded));
    await tester.pump();

    expect(find.text('Export DOT'), findsOneWidget);
    expect(find.text('Export XML'), findsOneWidget);
  });
}
