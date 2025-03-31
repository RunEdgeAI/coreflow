import "package:flutter/material.dart";

class Utils {
  static bool isPointNearEdge(Offset point, Offset start, Offset end) {
    final double threshold = 20.0;

    // Vector from start to end
    final vec = end - start;
    final length = vec.distance;

    // Vector from start to point
    final pointVec = point - start;

    // Calculate projection
    final t = (pointVec.dx * vec.dx + pointVec.dy * vec.dy) / (length * length);

    // Check if projection is within line segment
    if (t < 0 || t > 1) return false;

    // Calculate closest point on line
    final projection = Offset(
      start.dx + t * vec.dx,
      start.dy + t * vec.dy,
    );

    // Check distance to line
    return (point - projection).distance < threshold;
  } // End of _isPointNearEdge
}
