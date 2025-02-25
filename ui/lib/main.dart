import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:xml/xml.dart' as xml;
import 'dart:math';

void main() {
  runApp(GraphEditorApp());
}

class GraphEditorApp extends StatelessWidget {
  const GraphEditorApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Graph Editor',
      theme: ThemeData(
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
}

class Node {
  final int id;
  String name;
  Offset position;
  String kernel;
  String target;
  Node({
    required this.id,
    required this.name,
    required this.position,
    this.kernel = 'Default',
    this.target = 'Default'
  });
}

class Edge {
  final Node source;
  final Node target;
  Edge({
    required this.source,
    required this.target
  });
}

class Graph {
  List<Node> nodes;
  List<Edge> edges;
  Graph({
    required this.nodes,
    required this.edges
  });
}

class Kernel {
  final String name;
  final List<String> inputs;
  final List<String> outputs;

  Kernel({
    required this.name,
    required this.inputs,
    required this.outputs,
  });
}

class Target {
  final String name;
  final List<Kernel> kernels;

  Target({
    required this.name,
    required this.kernels,
  });
}

class GraphEditor extends StatefulWidget {
  const GraphEditor({super.key});

  @override
  GraphEditorState createState() => GraphEditorState();
}

class GraphEditorState extends State<GraphEditor> {
  List<Graph> graphs = [];
  int nextId = 0;
  Node? selectedNode;
  Node? draggingNode;
  Offset? dragOffset;
  final FocusNode _focusNode = FocusNode();
  Edge? selectedEdge;
  int selectedGraphIndex = 0;
  int? selectedGraphRow;
  final TextEditingController _nameController = TextEditingController();
  final FocusNode _nameFocusNode = FocusNode();
  // Supported targets/kernels loaded from XML.
  List<Target> _supported = [];

  @override
  void initState() {
    super.initState();
    _focusNode.requestFocus();
    _loadSupportedXml();
  }

  @override
  void dispose() {
    _focusNode.dispose();
    _nameController.dispose();
    _nameFocusNode.dispose();
    super.dispose();
  }

  Future<void> _loadSupportedXml() async {
    // Load the XML file from assets.
    final xmlString = await rootBundle.loadString('assets/supported.xml');
    final document = xml.XmlDocument.parse(xmlString);

    List<Target> supported = [];

    // Parse each <Target> element.
    for (var targetElement in document.findAllElements('Target')) {
      final targetName = targetElement.getAttribute('name') ?? 'Default';
      List<Kernel> kernels = [];

      // Parse each <Kernel> element within the <Target>.
      for (var kernelElement in targetElement.findElements('Kernel')) {
        final kernelName = kernelElement.getAttribute('name') ?? 'Unknown';
        List<String> inputs = [];
        List<String> outputs = [];

        // Parse <Inputs> and <Outputs> elements within the <Kernel>.
        final inputsElement = kernelElement.findElements('Inputs').firstOrNull;
        if (inputsElement != null) {
          inputs = inputsElement
              .findElements('Input')
              .map((element) => element.innerText.trim())
              .toList();
        }

        final outputsElement = kernelElement.findElements('Outputs').firstOrNull;
        if (outputsElement != null) {
          outputs = outputsElement
              .findElements('Output')
              .map((element) => element.innerText.trim())
              .toList();
        }

        kernels.add(Kernel(name: kernelName, inputs: inputs, outputs: outputs));
      }

      supported.add(Target(name: targetName, kernels: kernels));
    }

    setState(() {
      _supported = supported;
    });
  }

  void _addGraph() {
    setState(() {
      graphs.add(Graph(nodes: [], edges: []));
      selectedGraphIndex = graphs.length - 1;
    });
    _deselectAll();
  }

  void _deleteGraph(int index) {
    setState(() {
      graphs.removeAt(index);
      if (selectedGraphIndex >= graphs.length) {
        selectedGraphIndex = graphs.length - 1;
      }
    });
    _deselectAll();
  }

  void _addNode(Graph graph, Offset position, Size panelSize) {
    // Assuming the radius of the node is 25
    final nodeRadius = 25.0;
    final clampedX = position.dx.clamp(nodeRadius, panelSize.width - nodeRadius);
    final clampedY = position.dy.clamp(nodeRadius, panelSize.height - nodeRadius);
    final clampedPosition = Offset(clampedX, clampedY);

    setState(() {
      final newNode = Node(
        id: nextId++,
        name: 'Node $nextId',
        position: clampedPosition,
      );
      graph.nodes.add(newNode);
    });
    _deselectAll();
  }

  void _addEdge(Graph graph, Node source, Node target) {
    // Check if the source and target nodes are the same
    if (source == target) {
      return;
    }

    // Check if an edge already exists between the same pair of nodes
    bool edgeExists = graph.edges.any((edge) =>
      (edge.source == source && edge.target == target));

    if (!edgeExists) {
      setState(() {
        final newEdge = Edge(source: source, target: target);
        graph.edges.add(newEdge);
      });
      // Deselect selected node and any selected edge after creating an edge
      _deselectAll();
    }
  }

  void _deselectAll() {
    setState(() {
      selectedNode = null;
      selectedEdge = null;
      selectedGraphRow = null;
    });
  }

  void _deleteSelected(Graph graph) {
    setState(() {
      if (selectedNode != null) {
        graph.edges.removeWhere((edge) =>
          edge.source == selectedNode || edge.target == selectedNode);
        graph.nodes.remove(selectedNode);
        selectedNode = null;
      } else if (selectedEdge != null) {
        graph.edges.remove(selectedEdge);
      }
      _deselectAll();
    });
  }

  String _exportDOT(Graph graph) {
    StringBuffer dot = StringBuffer();
    dot.writeln('digraph G {');
    for (var node in graph.nodes) {
      dot.writeln('  node${node.id} [label="${node.name}"];');
    }
    for (var edge in graph.edges) {
      dot.writeln('  node${edge.source.id} -> node${edge.target.id};');
    }
    dot.writeln('}');
    return dot.toString();
  }

  String _exportXML(Graph graph) {
    StringBuffer xml = StringBuffer();
    xml.writeln('<Graph>');
    for (var node in graph.nodes) {
      xml.writeln(
          '  <Node id="${node.id}" name="${node.name}" x="${node.position.dx}" y="${node.position.dy}" />');
    }
    xml.writeln('  <Edges>');
    for (var edge in graph.edges) {
      xml.writeln('    <Edge source="${edge.source.id}" target="${edge.target.id}" />');
    }
    xml.writeln('  </Edges>');
    xml.writeln('</Graph>');
    return xml.toString();
  }

  List<String> _getUpstreamDependencies(Node node) {
    final graph = graphs[selectedGraphIndex];
    return graph.edges
        .where((edge) => edge.target == node)
        .map((edge) => edge.source.name)
        .toList();
  }

  List<String> _getDownstreamDependencies(Node node) {
    final graph = graphs[selectedGraphIndex];
    return graph.edges
        .where((edge) => edge.source == node)
        .map((edge) => edge.target.name)
        .toList();
  }

  Node? _findNodeAt(Graph graph, Offset position) {
    for (var node in graph.nodes.reversed) {
      if ((node.position - position).distance < 25) {
        return node;
      }
    }
    return null;
  }

  Edge? _findEdgeAt(Graph graph, Offset position) {
    for (var edge in graph.edges.reversed) {
      if (_isPointNearEdge(position, edge.source.position, edge.target.position)) {
        return edge;
      }
    }
    return null;
  }

  bool _isPointNearEdge(Offset point, Offset start, Offset end) {
    final double threshold = 10.0;

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
  }

  void _updateNameController() {
    if (selectedNode != null && _nameController.text != selectedNode!.name) {
      _nameController.text = selectedNode!.name;
    }
  }

  @override
  Widget build(BuildContext context) {
    _updateNameController();

    return Scaffold(
      appBar: AppBar(
        actions: [
          IconButton(
            icon: Icon(Icons.code),
            tooltip: 'Export DOT',
            onPressed: () {
              if (graphs.isNotEmpty) {
                // Export the selected graph
                final dot = _exportDOT(graphs[selectedGraphIndex]);
                showDialog(
                  context: context,
                  builder: (context) => AlertDialog(
                    title: Text("Export DOT"),
                    content: SingleChildScrollView(child: Text(dot)),
                    actions: [
                      TextButton(
                        onPressed: () => Navigator.of(context).pop(),
                        child: Text("Close"),
                      ),
                    ],
                  ),
                );
              }
            },
          ),
          IconButton(
            icon: Icon(Icons.code_off),
            tooltip: 'Export XML',
            onPressed: () {
              if (graphs.isNotEmpty) {
                // Export the selected graph
                final xml = _exportXML(graphs[selectedGraphIndex]);
                showDialog(
                  context: context,
                  builder: (context) => AlertDialog(
                    title: Text("Export XML"),
                    content: SingleChildScrollView(child: Text(xml)),
                    actions: [
                      TextButton(
                        onPressed: () => Navigator.of(context).pop(),
                        child: Text("Close"),
                      ),
                    ],
                  ),
                );
              }
            },
          ),
        ],
      ),
      body: KeyboardListener(
        focusNode: _focusNode,
        onKeyEvent: (event) {
          if (_nameFocusNode.hasFocus) return;

          if (event is KeyDownEvent) {
            if (event.logicalKey == LogicalKeyboardKey.backspace ||
                event.logicalKey == LogicalKeyboardKey.delete) {
              if (selectedGraphRow != null) {
                _deleteGraph(selectedGraphRow!);
              } else if (graphs.isNotEmpty) {
                _deleteSelected(graphs[selectedGraphIndex]);
              }
            } else if (event.logicalKey == LogicalKeyboardKey.escape) {
              _deselectAll();
            }
          }
        },
        child: Row(
          children: [
            // Left panel for graph list and add graph button
            Container(
              width: 200,
              color: Colors.grey[900],
              child: Column(
                children: [
                  IconButton(
                    icon: Icon(Icons.add),
                    tooltip: 'Add Graph',
                    onPressed: _addGraph,
                  ),
                  Expanded(
                    child: ListView.builder(
                      itemCount: graphs.length,
                      itemBuilder: (context, index) {
                        return ListTile(
                          title: Text('Graph ${index + 1}'),
                          selected: selectedGraphRow == index,
                          onTap: () {
                            setState(() {
                              selectedGraphIndex = index;
                              selectedGraphRow = index;
                              // Reset selected node when switching graphs
                              selectedNode = null;
                            });
                          },
                        );
                      },
                    ),
                  ),
                ],
              ),
            ),
            // Center panel for graph visualization and node/edge creation
            Expanded(
              child: LayoutBuilder(
                builder: (context, constraints) {
                  return Column(
                    children: [
                      Expanded(
                        child: graphs.isNotEmpty
                            ? GestureDetector(
                                onTapDown: (details) {
                                  final graph = graphs[selectedGraphIndex];
                                  final tappedNode = _findNodeAt(graph, details.localPosition);
                                  final tappedEdge = _findEdgeAt(graph, details.localPosition);
                                  setState(() {
                                    if (tappedNode != null) {
                                      // Deselect the selected edge
                                      selectedEdge = null;
                                      if (selectedNode == null) {
                                        selectedNode = tappedNode;
                                      } else {
                                        _addEdge(graph, selectedNode!, tappedNode);
                                        // Deselect the selected node
                                        selectedNode = null;
                                      }
                                    } else if (tappedEdge != null) {
                                      // Deselect the selected node
                                      selectedNode = null;
                                      // Select the tapped edge
                                      selectedEdge = tappedEdge;
                                    } else {
                                      _addNode(graph, details.localPosition, constraints.biggest);
                                      // Deselect the selected node
                                      selectedNode = null;
                                      // Deselect the selected edge
                                      selectedEdge = null;
                                      // Deselect the selected graph row
                                      selectedGraphRow = null;
                                    }
                                  });
                                },
                                onPanUpdate: (details) {
                                  if (draggingNode != null) {
                                    setState(() {
                                      final newPosition = draggingNode!.position + details.delta;
                                      // Assuming the radius of the node is 25
                                      final nodeRadius = 25.0;
                                      // Ensure the node stays within the bounds of the center panel
                                      if (newPosition.dx - nodeRadius >= 0 &&
                                          newPosition.dx + nodeRadius <= constraints.maxWidth &&
                                          newPosition.dy - nodeRadius >= 0 &&
                                          newPosition.dy + nodeRadius <= constraints.maxHeight) {
                                        draggingNode!.position = newPosition;
                                      }
                                    });
                                  }
                                },
                                onPanStart: (details) {
                                  setState(() {
                                    final graph = graphs[selectedGraphIndex];
                                    draggingNode = _findNodeAt(graph, details.localPosition);
                                    dragOffset = details.localPosition;
                                  });
                                },
                                onPanEnd: (details) {
                                  setState(() {
                                    draggingNode = null;
                                    dragOffset = null;
                                  });
                                },
                                child: CustomPaint(
                                  painter: GraphPainter(
                                    graphs[selectedGraphIndex].nodes,
                                    graphs[selectedGraphIndex].edges,
                                    selectedNode,
                                    selectedEdge,
                                  ),
                                  child: Container(),
                                ),
                              )
                            : Center(child: Text('No graphs available')),
                      ),
                    ],
                  );
                },
              ),
            ),
            // Right panel for node attributes
            AnimatedContainer(
              duration: Duration(milliseconds: 300),
              width: selectedNode != null ? 200 : 0,
              color: Colors.grey[800],
              child: selectedNode != null
                  ? ListView(
                      padding: EdgeInsets.all(8.0),
                      children: [
                        Text(
                          'Node Attributes',
                          style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                        ),
                        SizedBox(height: 8.0),
                        TextField(
                          controller: TextEditingController(text: selectedNode!.id.toString()),
                          decoration: InputDecoration(
                            labelText: 'ID',
                          ),
                          // Make ID field read-only
                          enabled: false,
                        ),
                        SizedBox(height: 8.0),
                        TextField(
                          controller: _nameController,
                          focusNode: _nameFocusNode,
                          decoration: InputDecoration(
                            labelText: 'Name',
                          ),
                          onChanged: (value) {
                            setState(() {
                              selectedNode!.name = value;
                            });
                          },
                          onEditingComplete: () {
                            _focusNode.requestFocus();
                          },
                        ),
                        SizedBox(height: 8.0),
                        DropdownButtonFormField<String>(
                          isExpanded: true,
                          value: (selectedNode!.target == 'Default') ?
                            _supported.first.name : selectedNode!.target,
                          decoration: InputDecoration(
                            constraints: BoxConstraints(maxWidth: 200),
                            labelText: 'Target',
                            isDense: true,
                          ),
                          items: _supported
                              .map((target) => DropdownMenuItem<String>(
                                    alignment: Alignment.centerLeft,
                                    value: target.name,
                                    child: Text(target.name),
                                  ))
                              .toList(),
                          onChanged: (newValue) {
                            setState(() {
                              selectedNode!.target = newValue!;
                              // Set kernel to the first available value if target changes.
                              final target = _supported.firstWhere((t) => t.name == newValue);
                              if (target.kernels.isNotEmpty) {
                                selectedNode!.kernel = target.kernels.first.name;
                              }
                            });
                          },
                        ),
                        SizedBox(height: 8.0),
                        DropdownButtonFormField<String>(
                          isExpanded: true,
                          value: (selectedNode!.kernel == 'Default') ?
                            _supported.first.kernels.first.name : selectedNode!.kernel,
                          decoration: InputDecoration(
                            constraints: BoxConstraints(maxWidth: 200),
                            labelText: 'Kernel',
                            isDense: true,
                          ),
                          items: _supported
                              .firstWhere((target) => target.name == selectedNode!.target,
                              orElse: () => _supported.first,
                              )
                              .kernels
                              .map((kernel) => DropdownMenuItem<String>(
                                    value: kernel.name,
                                    child: FittedBox(
                                      fit: BoxFit.scaleDown,
                                      alignment: Alignment.centerLeft,
                                      child: Text(
                                        kernel.name,
                                        style: TextStyle(fontSize: 12),
                                        overflow: TextOverflow.ellipsis,
                                      ),
                                    ),
                                  ))
                              .toList(),
                          onChanged: (newValue) {
                            setState(() {
                              selectedNode!.kernel = newValue!;
                            });
                          },
                        ),
                        SizedBox(height: 8.0),
                        Text(
                          'Upstream Dependencies',
                        ),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _getUpstreamDependencies(selectedNode!).map((dep) => TextField(
                            controller: TextEditingController(text: dep),
                            // Make Upstream dependencies read-only
                            enabled: false,
                          )).toList(),
                        ),
                        SizedBox(height: 8.0),
                        Text(
                          'Downstream Dependencies',
                        ),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _getDownstreamDependencies(selectedNode!).map((dep) => TextField(
                            controller: TextEditingController(text: dep),
                            // Make Downstream dependencies read-only
                            enabled: false,
                          )).toList(),
                        ),
                        SizedBox(height: 8.0),
                        Text(
                          'Inputs',
                        ),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _supported
                              .firstWhere((target) => target.name == selectedNode!.target,
                              orElse: () => _supported.first,
                              )
                              .kernels
                              .firstWhere((kernel) => kernel.name == selectedNode!.kernel,
                              orElse: () => _supported.first.kernels.first,
                              )
                              .inputs
                              .map((input) => Text(input))
                              .toList(),
                        ),
                        SizedBox(height: 8.0),
                        Text(
                          'Outputs',
                        ),
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: _supported
                              .firstWhere((target) => target.name == selectedNode!.target,
                              orElse: () => _supported.first,
                              )
                              .kernels
                              .firstWhere((kernel) => kernel.name == selectedNode!.kernel,
                              orElse: () => _supported.first.kernels.first,
                              )
                              .outputs
                              .map((output) => Text(output))
                              .toList(),
                        ),
                        // Add more attributes as needed
                      ],
                    )
                  : Container(),
            ),
          ],
        ),
      ),
    );
  }
}

class GraphPainter extends CustomPainter {
  final List<Node> nodes;
  final List<Edge> edges;
  final Node? selectedNode;
  final Edge? selectedEdge;

  GraphPainter(this.nodes, this.edges, this.selectedNode, this.selectedEdge);

  void _drawArrow(Canvas canvas, Offset start, Offset end, Paint paint) {
    final double arrowSize = 5.0;
    // 25 degrees in radians
    final double angle = 25.0 * (3.14159 / 180.0);

    // Calculate the direction vector
    var direction = (end - start);
    direction = direction / direction.distance;

    // Calculate arrow base point - move back from end by node radius + arrow size
    var basePoint = end - direction * (25 + arrowSize);

    // Calculate arrow points
    var leftPoint = basePoint + Offset(
      arrowSize * (direction.dy * cos(angle) - direction.dx * sin(angle)),
      arrowSize * (-direction.dx * cos(angle) - direction.dy * sin(angle))
    );
    var rightPoint = basePoint + Offset(
      arrowSize * (-direction.dy * cos(angle) - direction.dx * sin(angle)),
      arrowSize * (direction.dx * cos(angle) - direction.dy * sin(angle))
    );

    // Draw arrow line
    canvas.drawLine(start, basePoint, paint);

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
      // ..color = Colors.red
      ..color = Colors.blue.shade400
      ..style = PaintingStyle.fill;

    final edgePaint = Paint()
      // ..color = Colors.white.withOpacity(0.7)
      ..color = Color.alphaBlend(Colors.white.withAlpha(178), Colors.white)
      ..strokeWidth = 2
      ..style = PaintingStyle.stroke;

    // Update edge paint to include selection state
    final selectedEdgePaint = Paint()
      ..color = Colors.red
      ..strokeWidth = 3
      ..style = PaintingStyle.stroke;

    // Draw edges with selection highlighting
    for (var edge in edges) {
      final isSelected = edge == selectedEdge;
      final paint = isSelected ? selectedEdgePaint : edgePaint;

      // Draw glow effect for selected edge
      if (isSelected) {
        _drawArrow(canvas, edge.source.position, edge.target.position, Paint()
          // ..color = Color.alphaBlend(Colors.red.withAlpha(77), Colors.red)
          ..color = Color.alphaBlend(Colors.white.withAlpha(77), Colors.white)
          ..strokeWidth = 6
          ..style = PaintingStyle.stroke
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, 3));
      }

      _drawArrow(canvas, edge.source.position, edge.target.position, paint);
    }

    // Draw nodes with enhanced glow effect
    for (var node in nodes) {
      if (node == selectedNode) {
        // Enhanced glow effect for selected node
        canvas.drawCircle(node.position, 32, Paint()
          // ..color = Colors.red.withOpacity(0.4)
          ..color = Color.alphaBlend(Colors.blue.shade300.withAlpha(102), Colors.blue.shade300)
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, 12));

        canvas.drawCircle(node.position, 30, Paint()
          // ..color = Colors.red.withOpacity(0.3)
          ..color = Color.alphaBlend(Colors.blue.shade200.withAlpha(77), Colors.blue.shade200)
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, 8));
      } else {
        // Normal glow for unselected nodes
        canvas.drawCircle(node.position, 28, Paint()
          // ..color = Colors.blue.withOpacity(0.2)
          ..color = Color.alphaBlend(Colors.blue.withAlpha(51), Colors.blue)
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, 8));
      }

      // Draw main circle
      canvas.drawCircle(node.position, 25,
        node == selectedNode ? selectedNodePaint : nodePaint);

      // Draw stroke with enhanced highlight
      canvas.drawCircle(node.position, 25, Paint()
        ..color = node == selectedNode ? // Colors.white.withOpacity(0.8)
          Color.alphaBlend(Colors.white.withAlpha(204), Colors.white) : Colors.blue.shade300
        ..style = PaintingStyle.stroke
        ..strokeWidth = node == selectedNode ? 3 : 2);

      // Draw text with enhanced contrast for selected node
      final textSpan = TextSpan(
        text: node.name,
        style: TextStyle(
          color: node == selectedNode ? Colors.white : Color.alphaBlend(Colors.white.withAlpha(229), Colors.white),
          //  Colors.white.withOpacity(0.9),
          fontSize: node == selectedNode ? 14 : 12,
          fontWeight: node == selectedNode ? FontWeight.bold : FontWeight.w500,
          shadows: [
            Shadow(
              // color: Colors.black.withOpacity(0.5),
              color: Color.alphaBlend(Colors.black.withAlpha(127), Colors.black),
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
      final offset = node.position -
          Offset(textPainter.width / 2, textPainter.height / 2);
      textPainter.paint(canvas, offset);
    }
  }

  @override
  bool shouldRepaint(GraphPainter oldDelegate) => true;
}
