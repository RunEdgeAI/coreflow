import 'package:flutter/material.dart';

class GenerateButton extends StatelessWidget {
  final VoidCallback onPressed;
  const GenerateButton({super.key, required this.onPressed});

  @override
  Widget build(BuildContext context) {
    return Tooltip(
      message: 'Generate',
      child: GestureDetector(
        onTap: onPressed,
        child: Container(
          width: 44, // larger and perfectly circular
          height: 44,
          decoration: BoxDecoration(
            gradient: const LinearGradient(
              colors: [Color(0xFF00E1FF), Color(0xFFB400FF)],
            ),
            shape: BoxShape.circle,
          ),
          child: Container(
            margin: const EdgeInsets.all(3), // thin gradient border
            decoration: BoxDecoration(
              color: Color(0xFF2196F3),
              shape: BoxShape.circle,
            ),
            child: Center(
              child: Icon(Icons.auto_awesome, color: Colors.white, size: 24), // white icon for contrast
            ),
          ),
        ),
      ),
    );
  }
}