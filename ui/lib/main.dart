import 'package:flutter/material.dart';
import 'package:ui/graph_editor.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();

  runApp(GraphEditorApp());
}

class GraphEditorApp extends StatelessWidget {
  const GraphEditorApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Graph Editor',
      theme: ThemeData(
        useMaterial3: true,
        primarySwatch: Colors.blue,
        brightness: Brightness.dark,
        scaffoldBackgroundColor: Color(0xFF1a1a1a),
        appBarTheme: AppBarTheme(
          backgroundColor: Color(0xFF2196F3),
          elevation: 0,
        ),
      ),
      home: GraphEditor(),
    );
  }
} // End of GraphEditorApp
