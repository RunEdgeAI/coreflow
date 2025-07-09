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
      final String assetPath =
          String.fromCharCodes(message.buffer.asUint8List());
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

  test('Graph serialization/deserialization', () {
    // Create a simple node
    final node = Node(
      id: 1,
      name: 'TestNode',
      position: const Offset(10, 20),
      kernel: 'TestKernel',
      target: 'TestTarget',
      inputs: [],
      outputs: [],
    );
    // Create a graph with one node and no edges
    final graph = Graph(id: 42, nodes: [node], edges: []);

    // Serialize to JSON
    final json = graph.toJson();
    // Deserialize from JSON
    final graph2 = Graph.fromJson(json);

    // Check that the deserialized graph matches the original
    expect(graph2.id, graph.id);
    expect(graph2.nodes.length, 1);
    expect(graph2.nodes[0].name, 'TestNode');
    expect(graph2.nodes[0].position.dx, 10);
    expect(graph2.nodes[0].position.dy, 20);
    expect(graph2.edges.length, 0);

    // Add an edge and test again
    final node2 = Node(
      id: 2,
      name: 'TestNode2',
      position: const Offset(30, 40),
      kernel: 'TestKernel',
      target: 'TestTarget',
      inputs: [],
      outputs: [],
    );
    final edge = Edge(source: node, target: node2, srcId: 1, tgtId: 2);
    final graphWithEdge = Graph(id: 43, nodes: [node, node2], edges: [edge]);
    final json2 = graphWithEdge.toJson();
    final graphWithEdge2 = Graph.fromJson(json2);
    expect(graphWithEdge2.nodes.length, 2);
    expect(graphWithEdge2.edges.length, 1);
    expect(graphWithEdge2.edges[0].source.name, 'TestNode');
    expect(graphWithEdge2.edges[0].target.name, 'TestNode2');
    expect(graphWithEdge2.edges[0].srcId, 1);
    expect(graphWithEdge2.edges[0].tgtId, 2);
  });
}
