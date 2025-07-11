import 'package:flutter/material.dart';
import 'package:firebase_ai/firebase_ai.dart';
import 'package:flutter_ai_toolkit/flutter_ai_toolkit.dart'; // For LlmChatView, FirebaseProvider
import 'dart:convert';
import 'objects.dart'; // For Graph, ChatMessage
import 'package:flutter_ai_toolkit/src/providers/interface/attachments.dart'; // For Attachment

const double _aiPanelMinContentWidth = 260;
const double _aiPanelMaxWidth = 400;

class AiChatPanel extends StatelessWidget {
  final bool show;
  final FirebaseProvider? provider;
  final String systemPrompt;
  final Graph? currentGraph;
  final void Function(Graph newGraph)? onResponse;
  final VoidCallback? onClose;

  const AiChatPanel({
    Key? key,
    required this.show,
    required this.provider,
    required this.systemPrompt,
    required this.currentGraph,
    this.onResponse,
    this.onClose,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return AnimatedContainer(
      duration: Duration(milliseconds: 300),
      width: show ? _aiPanelMaxWidth : 0,
      curve: Curves.easeInOut,
      child: Container(
        color: Colors.grey[900],
        child: LayoutBuilder(
          builder: (context, constraints) {
            if (constraints.maxWidth < _aiPanelMinContentWidth) {
              return const SizedBox.shrink();
            }
            return AnimatedOpacity(
              duration: Duration(milliseconds: 200),
              opacity: show ? 1.0 : 0.0,
              curve: Curves.easeInOut,
              child: Column(
                children: [
                  Padding(
                    padding: const EdgeInsets.all(16.0),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text(
                          'AI Graph Assistant',
                          style: TextStyle(
                            fontSize: 20,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                        IconButton(
                          icon: Icon(Icons.close, color: Colors.white70),
                          onPressed: onClose,
                        ),
                      ],
                    ),
                  ),
                  Expanded(
                    child: provider == null
                        ? Center(child: Text('No AI provider'))
                        : GraphAwareChatView(
                            provider: provider!,
                            systemPrompt: systemPrompt,
                            currentGraph: currentGraph,
                            onResponse: onResponse,
                          ),
                  ),
                ],
              ),
            );
          },
        ),
      ),
    );
  }
}

class GraphAwareChatView extends StatefulWidget {
  final FirebaseProvider provider;
  final String systemPrompt;
  final Graph? currentGraph;
  final void Function(Graph newGraph)? onResponse;
  const GraphAwareChatView({
    required this.provider,
    required this.systemPrompt,
    this.currentGraph,
    this.onResponse,
  });

  @override
  State<GraphAwareChatView> createState() => _GraphAwareChatViewState();
}

class _GraphAwareChatViewState extends State<GraphAwareChatView> {
  String? _lastProcessedAiMsg;

  @override
  void initState() {
    super.initState();
    widget.provider.addListener(_onProviderUpdate);
  }

  @override
  void dispose() {
    widget.provider.removeListener(_onProviderUpdate);
    super.dispose();
  }

  void _onProviderUpdate() {
    final history = widget.provider.history.toList();
    ChatMessage? lastAiMsg;
    for (var i = history.length - 1; i >= 0; i--) {
      final msg = history[i];
      if (!msg.origin.isUser && (msg.text?.trim().isNotEmpty ?? false)) {
        lastAiMsg = msg;
        break;
      }
    }
    if (lastAiMsg != null && lastAiMsg.text != _lastProcessedAiMsg) {
      try {
        final jsonMap = jsonDecode(lastAiMsg.text!);
        final newGraph = Graph.fromJson(jsonMap);
        _lastProcessedAiMsg = lastAiMsg.text;
        if (widget.onResponse != null) {
          widget.onResponse!(newGraph);
        }
        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            const SnackBar(content: Text('Graph updated from AI!')),
          );
        }
      } catch (e, st) {
        if (lastAiMsg.text!.trim().startsWith('{')) {
          _lastProcessedAiMsg = lastAiMsg.text;
          if (mounted) {
            ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('Could not parse graph JSON: $e')),
            );
          }
        }
      }
    }
  }

  String _buildUserPrompt(String userMessage, Graph? currentGraph) {
    if (currentGraph == null) return userMessage;
    final graphJson = jsonEncode(currentGraph.toJson());
    return '''Current graph JSON:
$graphJson
\nUser request:\n$userMessage''';
  }

  @override
  Widget build(BuildContext context) {
    return LlmChatView(
      provider: widget.provider,
      messageSender: (String userMessage,
          {required Iterable<Attachment> attachments}) {
        final prompt = _buildUserPrompt(userMessage, widget.currentGraph);
        return widget.provider
            .sendMessageStream(prompt, attachments: attachments);
      },
      enableAttachments: false,
      enableVoiceNotes: false,
    );
  }
}