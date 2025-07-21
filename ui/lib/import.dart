import 'dart:convert';
import 'dart:io';
import 'dart:math';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:xml/xml.dart' as xml;
import 'objects.dart';

class ImportUtils {
  /// Generates a list of random, non-overlapping positions for nodes.
  static List<Offset> generateNodePositions(
    int count, {
    double width = 800,
    double height = 600,
    double margin = 60,
    double minDist = 30,
  }) {
    final rand = Random();
    final positions = <Offset>[];
    final maxTries = 100;
    for (int i = 0; i < count; i++) {
      Offset position;
      int tries = 0;
      do {
        position = Offset(
          margin + rand.nextDouble() * (width - 2 * margin),
          margin + rand.nextDouble() * (height - 2 * margin),
        );
        tries++;
      } while (positions.any((p) => (p - position).distance < minDist) &&
          tries < maxTries);
      positions.add(position);
    }
    return positions;
  }
}

class XmlImport {
  /// Parses an XML string and returns a [Graph] object.
  /// Assigns random, non-overlapping positions to nodes within the canvas.
  static Graph importGraph(
    String xmlString, {
    double width = 800,
    double height = 600,
  }) {
    final document = xml.XmlDocument.parse(xmlString);
    final graphElem = document.findAllElements('graph').first;
    final id = int.tryParse(graphElem.getAttribute('reference') ?? '0') ?? 0;
    final nodes = <Node>[];
    final edges = <Edge>[];
    final nodeMap = <int, Node>{};

    // Collect node elements first
    final nodeElems = graphElem.findAllElements('node').toList();
    final positions = ImportUtils.generateNodePositions(
      nodeElems.length,
      width: width,
      height: height,
    );

    for (int nodeIndex = 0; nodeIndex < nodeElems.length; nodeIndex++) {
      final nodeElem = nodeElems[nodeIndex];
      final nodeId =
          int.tryParse(nodeElem.getAttribute('reference') ?? '0') ?? 0;
      final kernelElem = nodeElem.findElements('kernel').firstOrNull;
      final kernel = kernelElem?.innerText ?? '';
      final position = positions[nodeIndex];
      final node = Node(
        id: nodeId,
        name: 'Node $nodeId',
        position: position,
        kernel: kernel,
        target: '',
        inputs: [],
        outputs: [],
      );
      nodes.add(node);
      nodeMap[nodeId] = node;
    }

    for (final edgeElem in graphElem.findAllElements('parameter')) {
      final srcNodeId = int.tryParse(edgeElem.getAttribute('node') ?? '') ?? -1;
      final srcId =
          int.tryParse(edgeElem.getAttribute('parameter') ?? '') ?? -1;
      final tgtId = int.tryParse(edgeElem.getAttribute('index') ?? '') ?? -1;
      if (srcNodeId != -1 && nodeMap.containsKey(srcNodeId)) {
        final targetNode = nodes.isNotEmpty ? nodes.last : nodeMap[srcNodeId];
        edges.add(
          Edge(
            source: nodeMap[srcNodeId]!,
            target: targetNode!,
            srcId: srcId,
            tgtId: tgtId,
          ),
        );
      }
    }

    return Graph(id: id, nodes: nodes, edges: edges);
  }
}

class JsonImport {
  static Graph importGraph(String jsonString) {
    final jsonMap = jsonDecode(jsonString);
    return Graph.fromJson(jsonMap);
  }
}

class DotImport {
  /// Parses a DOT string and returns a [Graph] object.
  /// Assigns random, non-overlapping positions to nodes within the canvas.
  static Graph? importGraph(
    String dotString, {
    double width = 800,
    double height = 600,
  }) {
    try {
      final nodeRegex = RegExp(r'\bnode(\d+) \[label="([^"]*)"\];');
      final edgeRegex = RegExp(r'\bnode(\d+) -> node(\d+);');
      final nodes = <int, Node>{};
      final edges = <Edge>[];
      // Collect node matches first
      final nodeMatches = nodeRegex.allMatches(dotString).toList();
      final positions = ImportUtils.generateNodePositions(
        nodeMatches.length,
        width: width,
        height: height,
      );
      for (int i = 0; i < nodeMatches.length; i++) {
        final match = nodeMatches[i];
        final id = int.parse(match.group(1)!);
        final name = match.group(2)!;
        final position = positions[i];
        final node = Node(
          id: id,
          name: name,
          position: position,
          kernel: '',
          target: '',
          inputs: [],
          outputs: [],
        );
        nodes[id] = node;
      }
      // Parse edges
      for (final match in edgeRegex.allMatches(dotString)) {
        final srcId = int.parse(match.group(1)!);
        final tgtId = int.parse(match.group(2)!);
        final srcNode = nodes[srcId];
        final tgtNode = nodes[tgtId];
        if (srcNode != null && tgtNode != null) {
          edges.add(
            Edge(source: srcNode, target: tgtNode, srcId: srcId, tgtId: tgtId),
          );
        }
      }
      if (nodes.isEmpty) {
        return null;
      }
      return Graph(id: 1, nodes: nodes.values.toList(), edges: edges);
    } catch (e) {
      return null;
    }
  }
}

Future<Graph?> showImportDialog(BuildContext context) async {
  final result = await FilePicker.platform.pickFiles(
    type: FileType.custom,
    allowedExtensions: ['xml', 'json', 'dot'],
    dialogTitle: 'Import Graph File',
  );

  if (result == null || result.files.isEmpty) {
    // User canceled
    return null;
  }

  final file = result.files.first;
  final path = file.path;
  if (path == null) {
    if (context.mounted) {
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(SnackBar(content: Text('No file selected.')));
    }
    return null;
  }

  try {
    final contents = await File(path).readAsString();
    final ext = path.split('.').last.toLowerCase();
    if (ext == 'xml') {
      return XmlImport.importGraph(contents);
    } else if (ext == 'json' || ext == 'dart') {
      return JsonImport.importGraph(contents);
    } else if (ext == 'dot') {
      return DotImport.importGraph(contents);
    } else {
      if (context.mounted) {
        ScaffoldMessenger.of(
          context,
        ).showSnackBar(SnackBar(content: Text('Unsupported file type: .$ext')));
      }
      return null;
    }
  } catch (e) {
    if (context.mounted) {
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(SnackBar(content: Text('Failed to import: $e')));
    }
    return null;
  }
}
