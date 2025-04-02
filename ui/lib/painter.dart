import 'dart:math';
import 'package:flutter/material.dart';
import 'package:ui/objects.dart';

class GraphPainter extends CustomPainter {
  final List<Node> nodes;
  final List<Edge> edges;
  final Node? selectedNode;
  final Edge? selectedEdge;
  final Offset? mousePosition;

  GraphPainter(this.nodes, this.edges, this.selectedNode, this.selectedEdge,
      this.mousePosition);

  void _drawArrow(Canvas canvas, Offset start, Offset end, Paint paint) {
    final double arrowSize = 5.0;
    // 25 degrees in radians
    final double angle = 25.0 * (pi / 180.0);

    // Calculate the direction vector
    var direction = (end - start);
    direction = direction / direction.distance;

    // Calculate arrow base point - move back from end by node radius + arrow size
    var basePoint = end - direction * arrowSize;

    // Calculate arrow points
    var leftPoint = basePoint +
        Offset(
            arrowSize * (direction.dy * cos(angle) - direction.dx * sin(angle)),
            arrowSize *
                (-direction.dx * cos(angle) - direction.dy * sin(angle)));
    var rightPoint = basePoint +
        Offset(
            arrowSize *
                (-direction.dy * cos(angle) - direction.dx * sin(angle)),
            arrowSize *
                (direction.dx * cos(angle) - direction.dy * sin(angle)));

    // Draw arrow line
    canvas.drawLine(start, end, paint);

    // Draw arrowhead
    final Path path = Path()
      ..moveTo(basePoint.dx, basePoint.dy)
      ..lineTo(leftPoint.dx, leftPoint.dy)
      ..lineTo(end.dx, end.dy)
      ..lineTo(rightPoint.dx, rightPoint.dy)
      ..close();
    canvas.drawPath(path, paint..style = PaintingStyle.fill);
  }

  @override
  void paint(Canvas canvas, Size size) {
    final nodePaint = Paint()
      ..color = Color(0xFF2196F3)
      ..style = PaintingStyle.fill
      ..maskFilter = MaskFilter.blur(BlurStyle.normal, 2);

    final selectedNodePaint = Paint()
      ..color = Colors.blue.shade400
      ..style = PaintingStyle.fill;

    final edgePaint = Paint()
      ..color = Color.alphaBlend(Colors.white.withAlpha(178), Colors.white)
      ..strokeWidth = 2
      ..style = PaintingStyle.stroke;

    // Update edge paint to include selection state
    final selectedEdgePaint = Paint()
      ..color = Colors.white70
      ..strokeWidth = 3
      ..style = PaintingStyle.stroke;

    // Draw edges with selection highlighting
    for (var edge in edges) {
      final isSelected = edge == selectedEdge;
      final paint = isSelected ? selectedEdgePaint : edgePaint;

      final sourceNode = edge.source;
      final targetNode = edge.target;

      final sourceAngle = (sourceNode.outputs.length == 1)
          ? 0
          : (pi / 4) +
              (sourceNode.outputs
                      .indexWhere((output) => output.id == edge.srcId) *
                  (3 * pi / 2) /
                  (sourceNode.outputs.length - 1));
      final sourceIconOffset = Offset(
        sourceNode.position.dx + 30 * cos(sourceAngle),
        sourceNode.position.dy + 30 * sin(sourceAngle),
      );

      final targetAngle = (targetNode.inputs.length == 1)
          ? pi
          : (3 * pi / 4) +
              (targetNode.inputs.indexWhere((input) => input.id == edge.tgtId) *
                  (pi / 2) /
                  (targetNode.inputs.length - 1));
      final targetIconOffset = Offset(
        targetNode.position.dx + 30 * cos(targetAngle),
        targetNode.position.dy + 30 * sin(targetAngle),
      );

      if (isSelected) {
        _drawArrow(
            canvas,
            sourceIconOffset,
            targetIconOffset,
            Paint()
              ..color =
                  Color.alphaBlend(Colors.white.withAlpha(77), Colors.white)
              ..strokeWidth = 6
              ..style = PaintingStyle.stroke
              ..maskFilter = MaskFilter.blur(BlurStyle.normal, 3));
      }

      _drawArrow(canvas, sourceIconOffset, targetIconOffset,
          paint); // Use icon offsets
    }

    // Draw nodes with enhanced glow effect
    for (var node in nodes) {
      final paint = node == selectedNode ? selectedNodePaint : nodePaint;

      if (node == selectedNode) {
        // Enhanced glow effect for selected node
        canvas.drawCircle(
            node.position,
            32,
            Paint()
              ..color = Color.alphaBlend(
                  Colors.blue.shade300.withAlpha(102), Colors.blue.shade300)
              ..maskFilter = MaskFilter.blur(BlurStyle.normal, 12));

        canvas.drawCircle(
            node.position,
            30,
            Paint()
              ..color = Color.alphaBlend(
                  Colors.blue.shade200.withAlpha(77), Colors.blue.shade200)
              ..maskFilter = MaskFilter.blur(BlurStyle.normal, 8));
      } else {
        // Normal glow for unselected nodes
        canvas.drawCircle(
            node.position,
            28,
            Paint()
              ..color = Color.alphaBlend(Colors.blue.withAlpha(51), Colors.blue)
              ..maskFilter = MaskFilter.blur(BlurStyle.normal, 8));
      }

      // Draw main circle
      canvas.drawCircle(node.position, 25, paint);
      // Draw stroke with enhanced highlight
      canvas.drawCircle(
          node.position,
          25,
          Paint()
            ..color = node == selectedNode
                ? // Colors.white.withOpacity(0.8)
                Color.alphaBlend(Colors.white.withAlpha(204), Colors.white)
                : Colors.blue.shade300
            ..style = PaintingStyle.stroke
            ..strokeWidth = node == selectedNode ? 3 : 2);

      // Draw text with enhanced contrast for selected node
      final textSpan = TextSpan(
        text: node.name,
        style: TextStyle(
          color: node == selectedNode
              ? Colors.white
              : Color.alphaBlend(Colors.white.withAlpha(229), Colors.white),
          //  Colors.white.withOpacity(0.9),
          fontSize: node == selectedNode ? 14 : 12,
          fontWeight: node == selectedNode ? FontWeight.bold : FontWeight.w500,
          shadows: [
            Shadow(
              color:
                  Color.alphaBlend(Colors.black.withAlpha(127), Colors.black),
              offset: Offset(0, 1),
              blurRadius: 3,
            ),
          ],
        ),
      );

      final textPainter = TextPainter(
        text: textSpan,
        textAlign: TextAlign.center,
        textDirection: TextDirection.ltr,
      );

      textPainter.layout(minWidth: 0, maxWidth: 50);
      final offset =
          node.position - Offset(textPainter.width / 2, textPainter.height / 2);
      textPainter.paint(canvas, offset);
    }
  }

  @override
  bool shouldRepaint(GraphPainter oldDelegate) => true;
}

class GridPainter extends CustomPainter {
  final double gridSize;
  final Color lineColor;

  GridPainter({this.gridSize = 20, this.lineColor = Colors.grey});

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = lineColor
      ..strokeWidth = 0.5;

    // Draw vertical lines
    for (double x = 0; x <= size.width; x += gridSize) {
      canvas.drawLine(Offset(x, 0), Offset(x, size.height), paint);
    }

    // Draw horizontal lines
    for (double y = 0; y <= size.height; y += gridSize) {
      canvas.drawLine(Offset(0, y), Offset(size.width, y), paint);
    }
  }

  @override
  bool shouldRepaint(covariant GridPainter oldDelegate) {
    return gridSize != oldDelegate.gridSize ||
        lineColor != oldDelegate.lineColor;
  }
}
