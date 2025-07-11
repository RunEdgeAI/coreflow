import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:ui/ai_panel.dart';

void main() {
  group('AiChatPanel', () {
    testWidgets('renders and hides with show=false', (WidgetTester tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: false,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
          ),
        ),
      );
      // Should have opacity 0.0
      final animatedOpacity = tester.widget<AnimatedOpacity>(find.byType(AnimatedOpacity));
      expect(animatedOpacity.opacity, equals(0.0));
    });

    testWidgets('renders and shows with show=true', (WidgetTester tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: true,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
          ),
        ),
      );
      // Should find header
      expect(find.text('AI Assistant'), findsOneWidget);
    });

    testWidgets('calls onClose when close button is tapped', (WidgetTester tester) async {
      bool closed = false;
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: true,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
            onClose: () => closed = true,
          ),
        ),
      );
      await tester.tap(find.byIcon(Icons.close));
      expect(closed, isTrue);
    });

    testWidgets('shows No AI provider if provider is null', (WidgetTester tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: true,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
          ),
        ),
      );
      expect(find.text('No AI provider'), findsOneWidget);
    });

    testWidgets('animates panel visibility when show changes', (WidgetTester tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: false,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
          ),
        ),
      );

      // Initially hidden - opacity should be 0
      final initialOpacity = tester.widget<AnimatedOpacity>(find.byType(AnimatedOpacity));
      expect(initialOpacity.opacity, equals(0.0));

      // Change to show
      await tester.pumpWidget(
        MaterialApp(
          home: AiChatPanel(
            show: true,
            provider: null,
            systemPrompt: '',
            currentGraph: null,
          ),
        ),
      );

      // Should animate to visible - opacity should be 1
      final finalOpacity = tester.widget<AnimatedOpacity>(find.byType(AnimatedOpacity));
      expect(finalOpacity.opacity, equals(1.0));
    });
  });
}