import 'dart:convert';
import 'objects.dart';
import 'package:flutter/material.dart';
import 'package:flutter_ai_toolkit/flutter_ai_toolkit.dart';
import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:google_fonts/google_fonts.dart';

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
    super.key,
    required this.show,
    required this.provider,
    required this.systemPrompt,
    required this.currentGraph,
    this.onResponse,
    this.onClose,
  });

  @override
  Widget build(BuildContext context) {
    return AnimatedContainer(
      duration: Duration(milliseconds: 300),
      width: show ? _aiPanelMaxWidth : 0,
      curve: Curves.easeInOut,
      child: Container(
        color: Colors.grey[800],
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
                    padding: const EdgeInsets.symmetric(
                      horizontal: 8.0,
                      vertical: 4.0,
                    ),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Expanded(
                          child: Center(
                            child: Text(
                              'AI Assistant',
                              style: TextStyle(
                                fontSize: 15,
                                fontWeight: FontWeight.bold,
                                color: Colors.white,
                              ),
                            ),
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
                    child:
                        provider == null
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
    super.key,
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
            const SnackBar(content: Text('Graph updated by AI Assistant!')),
          );
        }
      } catch (e) {
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
      style: darkChatViewStyle(),
      messageSender: (
        String userMessage, {
        required Iterable<Attachment> attachments,
      }) {
        final prompt = _buildUserPrompt(userMessage, widget.currentGraph);
        return widget.provider.sendMessageStream(
          prompt,
          attachments: attachments,
        );
      },
      enableAttachments: false,
      enableVoiceNotes: false,
    );
  }
}

// Copyright 2024 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

LlmChatViewStyle darkChatViewStyle() {
  final style = LlmChatViewStyle.defaultStyle();
  return LlmChatViewStyle(
    backgroundColor: _invertColor(style.backgroundColor),
    menuColor: Colors.grey.shade800,
    progressIndicatorColor: _invertColor(style.progressIndicatorColor),
    userMessageStyle: _darkUserMessageStyle(),
    llmMessageStyle: _darkLlmMessageStyle(),
    chatInputStyle: _darkChatInputStyle(),
    addButtonStyle: _darkActionButtonStyle(ActionButtonType.add),
    attachFileButtonStyle: _darkActionButtonStyle(ActionButtonType.attachFile),
    cameraButtonStyle: _darkActionButtonStyle(ActionButtonType.camera),
    stopButtonStyle: _darkActionButtonStyle(ActionButtonType.stop),
    recordButtonStyle: _darkActionButtonStyle(ActionButtonType.record),
    submitButtonStyle: _darkActionButtonStyle(ActionButtonType.submit),
    closeMenuButtonStyle: _darkActionButtonStyle(ActionButtonType.closeMenu),
    actionButtonBarDecoration: _invertDecoration(
      style.actionButtonBarDecoration,
    ),
    fileAttachmentStyle: _darkFileAttachmentStyle(),
    suggestionStyle: _darkSuggestionStyle(),
    closeButtonStyle: _darkActionButtonStyle(ActionButtonType.close),
    cancelButtonStyle: _darkActionButtonStyle(ActionButtonType.cancel),
    copyButtonStyle: _darkActionButtonStyle(ActionButtonType.copy),
    editButtonStyle: _darkActionButtonStyle(ActionButtonType.edit),
    galleryButtonStyle: _darkActionButtonStyle(ActionButtonType.gallery),
  );
}

UserMessageStyle _darkUserMessageStyle() {
  final style = UserMessageStyle.defaultStyle();
  return UserMessageStyle(
    textStyle: _invertTextStyle(style.textStyle),
    // inversion doesn't look great here
    // decoration: invertDecoration(style.decoration),
    decoration: (style.decoration! as BoxDecoration).copyWith(
      color: _greyBackground,
    ),
  );
}

LlmMessageStyle _darkLlmMessageStyle() {
  final style = LlmMessageStyle.defaultStyle();
  return LlmMessageStyle(
    icon: style.icon,
    iconColor: _invertColor(style.iconColor),
    // inversion doesn't look great here
    // iconDecoration: invertDecoration(style.iconDecoration),
    iconDecoration: BoxDecoration(
      color: _greyBackground,
      shape: BoxShape.circle,
    ),
    markdownStyle: _invertMarkdownStyle(style.markdownStyle),
    decoration: _invertDecoration(style.decoration),
  );
}

ChatInputStyle _darkChatInputStyle() {
  final style = ChatInputStyle.defaultStyle();
  return ChatInputStyle(
    decoration: _invertDecoration(style.decoration),
    textStyle: _invertTextStyle(style.textStyle),
    // inversion doesn't look great here
    // hintStyle: invertTextStyle(style.hintStyle),
    hintStyle: GoogleFonts.roboto(
      color: _greyBackground,
      fontSize: 14,
      fontWeight: FontWeight.w400,
    ),
    hintText: style.hintText,
    backgroundColor: _invertColor(style.backgroundColor),
  );
}

ActionButtonStyle _darkActionButtonStyle(ActionButtonType type) {
  final style = ActionButtonStyle.defaultStyle(type);
  return ActionButtonStyle(
    icon: style.icon,
    iconColor: _invertColor(style.iconColor),
    iconDecoration: switch (type) {
      ActionButtonType.add ||
      ActionButtonType.record ||
      ActionButtonType.stop => BoxDecoration(
        color: _greyBackground,
        shape: BoxShape.circle,
      ),
      _ => _invertDecoration(style.iconDecoration),
    },
    text: style.text,
    textStyle: _invertTextStyle(style.textStyle),
  );
}

FileAttachmentStyle _darkFileAttachmentStyle() {
  final style = FileAttachmentStyle.defaultStyle();
  return FileAttachmentStyle(
    // inversion doesn't look great here
    // decoration: invertDecoration(style.decoration),
    decoration: ShapeDecoration(
      color: _greyBackground,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
    ),
    icon: style.icon,
    iconColor: _invertColor(style.iconColor),
    iconDecoration: _invertDecoration(style.iconDecoration),
    filenameStyle: _invertTextStyle(style.filenameStyle),
    // inversion doesn't look great here
    // filetypeStyle: invertTextStyle(style.filetypeStyle),
    filetypeStyle: style.filetypeStyle!.copyWith(color: Colors.black),
  );
}

SuggestionStyle _darkSuggestionStyle() {
  final style = SuggestionStyle.defaultStyle();
  return SuggestionStyle(
    textStyle: _invertTextStyle(style.textStyle),
    decoration: BoxDecoration(
      color: _greyBackground,
      borderRadius: BorderRadius.all(Radius.circular(8)),
    ),
  );
}

const Color _greyBackground = Color(0xFF535353);

Color? _invertColor(Color? color) =>
    color != null
        ? Color.from(
          alpha: color.a,
          red: 1 - color.r,
          green: 1 - color.g,
          blue: 1 - color.b,
        )
        : null;

Decoration _invertDecoration(Decoration? decoration) => switch (decoration!) {
  final BoxDecoration d => d.copyWith(color: _invertColor(d.color)),
  final ShapeDecoration d => ShapeDecoration(
    color: _invertColor(d.color),
    shape: d.shape,
    shadows: d.shadows,
    image: d.image,
    gradient: d.gradient,
  ),
  _ => decoration,
};

TextStyle _invertTextStyle(TextStyle? style) =>
    style!.copyWith(color: _invertColor(style.color));

MarkdownStyleSheet? _invertMarkdownStyle(MarkdownStyleSheet? markdownStyle) =>
    markdownStyle?.copyWith(
      a: _invertTextStyle(markdownStyle.a),
      blockquote: _invertTextStyle(markdownStyle.blockquote),
      checkbox: _invertTextStyle(markdownStyle.checkbox),
      code: _invertTextStyle(markdownStyle.code),
      del: _invertTextStyle(markdownStyle.del),
      em: _invertTextStyle(markdownStyle.em),
      strong: _invertTextStyle(markdownStyle.strong),
      p: _invertTextStyle(markdownStyle.p),
      tableBody: _invertTextStyle(markdownStyle.tableBody),
      tableHead: _invertTextStyle(markdownStyle.tableHead),
      h1: _invertTextStyle(markdownStyle.h1),
      h2: _invertTextStyle(markdownStyle.h2),
      h3: _invertTextStyle(markdownStyle.h3),
      h4: _invertTextStyle(markdownStyle.h4),
      h5: _invertTextStyle(markdownStyle.h5),
      h6: _invertTextStyle(markdownStyle.h6),
      listBullet: _invertTextStyle(markdownStyle.listBullet),
      img: _invertTextStyle(markdownStyle.img),
    );
