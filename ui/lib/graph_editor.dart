import 'ai_panel.dart';
import 'dart:math';
import 'export.dart';
import 'generate_button.dart';
import 'objects.dart';
import 'package:firebase_ai/firebase_ai.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_ai_toolkit/flutter_ai_toolkit.dart';
import 'package:xml/xml.dart' as xml;
import 'painter.dart';

class GraphEditor extends StatefulWidget {
  const GraphEditor({super.key});

  @override
  GraphEditorState createState() => GraphEditorState();
} // End of GraphEditor

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
  Offset? mousePosition;
  Node? edgeStartNode;
  int? edgeStartOutput;
  int _refCount = 0;
  bool _showChatModal = false;
  FirebaseProvider? _aiProvider;

  // Public getter to check if XML is loaded
  bool get isXmlLoaded => _supported.isNotEmpty;

  // Future that completes when XML is loaded
  late final Future<void> xmlLoaded;

  @override
  void initState() {
    super.initState();
    _focusNode.requestFocus();
    xmlLoaded = _loadSupportedXml();
  } // End of initState

  @override
  void dispose() {
    _focusNode.dispose();
    _nameController.dispose();
    _nameFocusNode.dispose();
    super.dispose();
  } // End of dispose

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
              .map((element) => element.innerText.trim().replaceAll('VX_', ''))
              .toList();
        }

        final outputsElement =
            kernelElement.findElements('Outputs').firstOrNull;
        if (outputsElement != null) {
          outputs = outputsElement
              .findElements('Output')
              .map((element) => element.innerText.trim().replaceAll('VX_', ''))
              .toList();
        }

        kernels.add(Kernel(name: kernelName, inputs: inputs, outputs: outputs));
      }

      supported.add(Target(name: targetName, kernels: kernels));
    }

    setState(() {
      _supported = supported;
    });
  } // End of _loadSupportedXml

  void _addGraph() {
    setState(() {
      graphs.add(Graph(id: _refCount++, nodes: [], edges: []));
      selectedGraphIndex = graphs.length - 1;
    });
    _deselectAll();
    _restoreMainFocus();
  } // End of _addGraph

  void _deleteGraph(int index) {
    setState(() {
      graphs.removeAt(index);
      if (selectedGraphIndex >= graphs.length) {
        selectedGraphIndex = graphs.length - 1;
      }
    });
    _deselectAll();
    _refCount--;
    _restoreMainFocus();
  } // End of _deleteGraph

  void _addNode(Graph graph, Offset position, Size panelSize) {
    // Assuming the radius of the node is 25
    final nodeRadius = 25.0;
    final clampedX =
        position.dx.clamp(nodeRadius, panelSize.width - nodeRadius);
    final clampedY =
        position.dy.clamp(nodeRadius, panelSize.height - nodeRadius);
    final clampedPosition = Offset(clampedX, clampedY);

    setState(() {
      final newNode = Node(
        id: _refCount++,
        name: 'Node $nextId',
        position: clampedPosition,
        target: _supported.first.name,
        kernel: _supported.first.kernels.first.name,
      );
      _updateNodeIO(newNode, newNode.kernel);
      nextId++;
      graph.nodes.add(newNode);
    });
    _deselectAll();
    _restoreMainFocus();
  } // End of _addNode

  void _addEdge(Graph graph, Node source, Node target, int srcId, int tgtId) {
    // Check if the source and target nodes are the same
    if (source == target) {
      return;
    }

    // Check if an edge already exists between the same pair of nodes
    bool edgeExists = graph.edges.any((edge) => (edge.source == source &&
        edge.target == target &&
        edge.srcId == srcId &&
        edge.tgtId == tgtId));

    if (!edgeExists) {
      setState(() {
        final index = target.inputs.indexWhere((input) => input.id == tgtId);
        if (index >= 0 && index < target.inputs.length) {
          // If an input was tapped, update its linkId to be source.id.
          // target.inputs[index] = source.outputs.firstWhere((output) => output.id == srcId);
          target.inputs[index].linkId = srcId;
          // Create a new edge
          final newEdge =
              Edge(source: source, target: target, srcId: srcId, tgtId: tgtId);
          graph.edges.add(newEdge);
        }
      });
      // Deselect selected node and any selected edge after creating an edge
      _deselectAll();
      _restoreMainFocus();
    }
  } // End of _addEdge

  void _deselectAll() {
    setState(() {
      selectedNode = null;
      selectedEdge = null;
      selectedGraphRow = null;
      edgeStartNode = null;
      edgeStartOutput = null;
    });
    _restoreMainFocus();
  } // End of _deselectAll

  void _deleteSelected(Graph graph) {
    setState(() {
      if (selectedNode != null) {
        // First remove all edges connected to this node
        graph.edges.removeWhere((edge) =>
            edge.source == selectedNode || edge.target == selectedNode);

        // Decrement reference count for all inputs and outputs
        _refCount -= selectedNode!.inputs.length;
        _refCount -= selectedNode!.outputs.length;

        // Decrement reference count for the node itself
        _refCount--;

        // Remove the node from the graph
        graph.nodes.remove(selectedNode);
        selectedNode = null;
      } else if (selectedEdge != null) {
        graph.edges.remove(selectedEdge);
      }
      _deselectAll();
    });
    _restoreMainFocus();
  } // End of _deleteSelected

  void _updateNameController() {
    if (selectedNode != null && _nameController.text != selectedNode!.name) {
      _nameController.text = selectedNode!.name;
    }
  } // End of _updateNameController

  void _updateNodeIO(Node node, String kernelName) {
    final target = _supported.firstWhere((t) => t.name == node.target);
    final kernel = target.kernels.firstWhere((k) => k.name == kernelName);

    setState(() {
      // Decrement reference count for old inputs and outputs
      _refCount -= node.inputs.length;
      _refCount -= node.outputs.length;

      // Create new inputs and outputs
      node.inputs = kernel.inputs
          .map((input) => Reference.createReference(input, _refCount++))
          .toList();
      node.outputs = kernel.outputs
          .map((output) => Reference.createReference(output, _refCount++))
          .toList();
      _buildTooltips();
    });
  } // End of _updateNodeIO

  String _buildSystemPrompt(List<Target> supportedTargets) {
    final buffer = StringBuffer();
    buffer.writeln("You are an expert AI assistant for a visual graph editor. "
        "The user will describe a graph, and you will generate a JSON object representing the graph. "
        "Use only the following supported targets and kernels. "
        "Return only the JSON for the graph, matching the schema below. "
        "Do not include any explanation or markdown formatting. "
        "All node positions should be unique and within a 2D space (e.g., dx and dy between 0 and 500). "
        "If a graph is already defined, preserve the position (offset) of each existing node in the output JSON, unless the user specifically requests a layout change. "
        "For new nodes, assign a position that does not overlap with existing nodes.");
    buffer.writeln("\nSupported Targets and Kernels:");
    for (final target in supportedTargets) {
      buffer.writeln("- Target: ${target.name}");
      for (final kernel in target.kernels) {
        buffer.writeln(
            "  - Kernel: ${kernel.name} (inputs: ${kernel.inputs.join(', ')}, outputs: ${kernel.outputs.join(', ')})");
      }
    }
    buffer.writeln("\nJSON schema example:");
    buffer.writeln('''
{
  "id": 1,
  "nodes": [
    {
      "id": 1,
      "name": "A",
      "position": {"dx": 100, "dy": 100},
      "kernel": "add",
      "target": "CPU",
      "inputs": [],
      "outputs": []
    }
  ],
  "edges": [
    {"source": 1, "target": 2, "srcId": 1, "tgtId": 2}
  ]
}
''');
    buffer.writeln("Only use the kernels and targets listed above. "
        "Return only the JSON for the graph, with no extra text or formatting.");
    return buffer.toString();
  }

  void _openChatModal(String systemPrompt) {
    // Always create a new provider with the latest system prompt
    _aiProvider = FirebaseProvider(
      model: FirebaseAI.googleAI().generativeModel(
        model: 'gemini-2.5-flash',
        systemInstruction: Content.text(systemPrompt),
      ),
    );
    setState(() {
      _showChatModal = true;
    });
  }

  void _restoreMainFocus() {
    FocusScope.of(context).requestFocus(_focusNode);
  }

  void _exportDot(BuildContext context) {
    DotExport(graphs: graphs, graphIndex: selectedGraphIndex).export(context);
    _restoreMainFocus();
  }

  void _exportXml(BuildContext context) {
    XmlExport(
      graphs: graphs,
      graphIndex: selectedGraphIndex,
      refCount: _refCount,
    ).export(context);
    _restoreMainFocus();
  }

  @override
  Widget build(BuildContext context) {
    _updateNameController();
    final systemPrompt = _buildSystemPrompt(_supported);

    return Scaffold(
      appBar: AppBar(
        centerTitle: true,
        title: Text(
          'Edge Studio',
          style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
        ),
        actions: [
          PopupMenuButton<String>(
            icon: Icon(Icons.code_rounded), // Single export icon
            tooltip: 'Export',
            onSelected: (value) {
              if (value == 'Export DOT') {
                _exportDot(context);
              } else if (value == 'Export XML') {
                _exportXml(context);
              }
            },
            itemBuilder: (context) => [
              PopupMenuItem(
                value: 'Export DOT',
                child: Text('Export DOT'),
              ),
              PopupMenuItem(
                value: 'Export XML',
                child: Text('Export XML'),
              ),
            ],
          ),
        ],
      ),
      body: Column(
        children: [
          // Panel for graph list and 'add graph' button
          GraphListPanel(
              graphs: graphs,
              selectedGraphRow: selectedGraphRow,
              onAddGraph: _addGraph,
              onSelectGraph: (int index) {
                setState(() {
                  selectedGraphIndex = index;
                  selectedGraphRow = index;
                  // Reset selected node when switching graphs
                  selectedNode = null;
                });
                _restoreMainFocus();
              }),
          // Main area for graph visualization and interaction
          Expanded(
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                // AI Chat Panel (left)
                AiChatPanel(
                  show: _showChatModal,
                  provider: _aiProvider,
                  systemPrompt: systemPrompt,
                  currentGraph:
                      graphs.isNotEmpty ? graphs[selectedGraphIndex] : null,
                  onResponse: (Graph newGraph) {
                    setState(() {
                      if (graphs.isNotEmpty) {
                        graphs[selectedGraphIndex] = newGraph;
                      } else {
                        graphs.add(newGraph);
                        selectedGraphIndex = 0;
                      }
                    });
                    _restoreMainFocus();
                  },
                  onClose: () {
                    setState(() => _showChatModal = false);
                    _restoreMainFocus();
                  },
                ),
                // Main graph area (center)
                Expanded(
                  child: Stack(
                    children: [
                      // Center panel for graph visualization and node/edge creation
                      LayoutBuilder(
                        builder: (context, constraints) {
                          return Stack(
                            children: [
                              // Draw the grid background.
                              Positioned.fill(
                                child: CustomPaint(
                                  painter: GridPainter(
                                      gridSize: 60,
                                      lineColor: Colors.grey.withAlpha(76)),
                                ),
                              ),
                              graphs.isNotEmpty
                                  ? KeyboardListener(
                                      focusNode: _focusNode,
                                      onKeyEvent: (event) {
                                        if (_nameFocusNode.hasFocus) return;

                                        if (event is KeyDownEvent) {
                                          if (event.logicalKey ==
                                                  LogicalKeyboardKey
                                                      .backspace ||
                                              event.logicalKey ==
                                                  LogicalKeyboardKey.delete) {
                                            if (selectedGraphRow != null) {
                                              _deleteGraph(selectedGraphRow!);
                                            } else if (graphs.isNotEmpty) {
                                              _deleteSelected(
                                                  graphs[selectedGraphIndex]);
                                            }
                                          } else if (event.logicalKey ==
                                              LogicalKeyboardKey.escape) {
                                            _deselectAll();
                                          }
                                        }
                                      },
                                      child: MouseRegion(
                                        onHover: (event) {
                                          setState(() {
                                            mousePosition = event.localPosition;
                                          });
                                        },
                                        onExit: (event) {
                                          setState(() {
                                            mousePosition = null;
                                          });
                                        },
                                        child: GestureDetector(
                                          onTapDown: (details) {
                                            final graph =
                                                graphs[selectedGraphIndex];
                                            final tappedNode = graph.findNodeAt(
                                                details.localPosition);
                                            final tappedEdge = graph.findEdgeAt(
                                                details.localPosition);
                                            setState(() {
                                              if (tappedNode != null) {
                                                // Deselect the selected edge
                                                selectedEdge = null;
                                                if (selectedNode == null) {
                                                  selectedNode = tappedNode;
                                                } else {
                                                  // Deselect the selected node
                                                  selectedNode = null;
                                                }
                                              } else if (tappedEdge != null) {
                                                if (selectedEdge ==
                                                    tappedEdge) {
                                                  // Deselect the tapped edge if it is already selected
                                                  selectedEdge = null;
                                                } else {
                                                  // Deselect the selected node
                                                  selectedNode = null;
                                                  // Select the tapped edge
                                                  selectedEdge = tappedEdge;
                                                }
                                              } else {
                                                _addNode(
                                                    graph,
                                                    details.localPosition,
                                                    constraints.biggest);
                                                // Deselect the selected node
                                                selectedNode = null;
                                                // Deselect the selected edge
                                                selectedEdge = null;
                                                // Deselect the selected graph row
                                                selectedGraphRow = null;
                                                edgeStartNode = null;
                                                edgeStartOutput = null;
                                              }
                                            });
                                          },
                                          onPanUpdate: (details) {
                                            setState(() {
                                              mousePosition =
                                                  details.localPosition;
                                              if (draggingNode != null) {
                                                final newPosition =
                                                    draggingNode!.position +
                                                        details.delta;
                                                // Assuming the radius of the node is 25
                                                final nodeRadius = 25.0;
                                                // Ensure the node stays within the bounds of the center panel
                                                if (newPosition.dx - nodeRadius >= 0 &&
                                                    newPosition.dx +
                                                            nodeRadius <=
                                                        constraints.maxWidth -
                                                            (selectedNode !=
                                                                    null
                                                                ? 240
                                                                : 0) &&
                                                    newPosition.dy -
                                                            nodeRadius >=
                                                        0 &&
                                                    newPosition.dy +
                                                            nodeRadius <=
                                                        constraints.maxHeight) {
                                                  draggingNode!.position =
                                                      newPosition;
                                                }
                                              }
                                            });
                                          },
                                          onPanStart: (details) {
                                            setState(() {
                                              final graph =
                                                  graphs[selectedGraphIndex];
                                              draggingNode = graph.findNodeAt(
                                                  details.localPosition);
                                              dragOffset =
                                                  details.localPosition;
                                            });
                                          },
                                          onPanEnd: (details) {
                                            setState(() {
                                              draggingNode = null;
                                              dragOffset = null;
                                              edgeStartNode = null;
                                              edgeStartOutput = null;
                                              mousePosition = null;
                                            });
                                          },
                                          child: CustomPaint(
                                            painter: graphs.isNotEmpty
                                                ? GraphPainter(
                                                    graphs[selectedGraphIndex]
                                                        .nodes,
                                                    graphs[selectedGraphIndex]
                                                        .edges,
                                                    selectedNode,
                                                    selectedEdge,
                                                    mousePosition,
                                                  )
                                                : null,
                                            child: Container(),
                                          ),
                                        ),
                                      ),
                                    )
                                  : Center(child: Text('No graphs available')),
                              ..._buildTooltips(),
                              // Right panel for node attributes (overlay style)
                              Positioned(
                                top: 0,
                                right: 0,
                                bottom: 0,
                                child: AnimatedSlide(
                                  duration: Duration(milliseconds: 300),
                                  offset:
                                      Offset(selectedNode != null ? 0 : 1, 0),
                                  child: AnimatedOpacity(
                                    duration: Duration(milliseconds: 300),
                                    opacity: selectedNode != null ? 1.0 : 0.0,
                                    child: Container(
                                      width: 220,
                                      color: Colors.grey[800],
                                      child: selectedNode != null
                                          ? NodeAttributesPanel(
                                              graph: graphs.isNotEmpty
                                                  ? graphs[selectedGraphIndex]
                                                  : null,
                                              selectedNode: selectedNode,
                                              supportedTargets: _supported,
                                              nameController: _nameController,
                                              nameFocusNode: _nameFocusNode,
                                              onNameChanged: (value) {
                                                setState(() {
                                                  selectedNode!.name = value;
                                                });
                                              },
                                              onTargetChanged: (newValue) {
                                                setState(() {
                                                  selectedNode!.target =
                                                      newValue;
                                                  final target = _supported
                                                      .firstWhere((t) =>
                                                          t.name == newValue);
                                                  if (target
                                                      .kernels.isNotEmpty) {
                                                    selectedNode!.kernel =
                                                        target
                                                            .kernels.first.name;
                                                    _updateNodeIO(selectedNode!,
                                                        selectedNode!.kernel);
                                                  }
                                                });
                                              },
                                              onKernelChanged: (newValue) {
                                                setState(() {
                                                  selectedNode!.kernel =
                                                      newValue;
                                                  _updateNodeIO(
                                                      selectedNode!, newValue);
                                                });
                                              },
                                              onNameEditComplete:
                                                  _restoreMainFocus,
                                            )
                                          : null,
                                    ),
                                  ),
                                ),
                              ),
                              // Place the Generate button
                              Positioned(
                                bottom: 24,
                                left: 24,
                                child: GenerateButton(
                                  onPressed: () {
                                    setState(() {
                                      if (_showChatModal) {
                                        _showChatModal = false;
                                      } else {
                                        _openChatModal(systemPrompt);
                                      }
                                    });
                                  },
                                ),
                              ),
                            ],
                          );
                        },
                      ),
                    ],
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  } // End of build method

  List<Widget> _buildTooltips() {
    final tooltips = <Widget>[];
    int? edgeEndInput;
    if (graphs.isNotEmpty) {
      for (var node in graphs[selectedGraphIndex].nodes) {
        for (int i = 0; i < node.inputs.length; i++) {
          // Distribute the inputs from radians 3pi/4 to 5pi/4 aroud the node.
          // If there is only one input, it should be at pi radians.
          final angle = (node.inputs.length == 1)
              ? pi
              : (3 * pi / 4) + (i * (pi / 2) / (node.inputs.length - 1));
          final iconOffset = Offset(
            node.position.dx + 30 * cos(angle),
            node.position.dy + 30 * sin(angle),
          );
          tooltips.add(Positioned(
            left: iconOffset.dx - 8,
            top: iconOffset.dy - 8,
            child: GestureDetector(
              onTapDown: (details) {
                setState(() {
                  if (edgeStartNode != null && edgeStartOutput != null) {
                    final graph = graphs[selectedGraphIndex];
                    _addEdge(graph, edgeStartNode!, node, edgeStartOutput!,
                        node.inputs[i].id);
                    edgeStartNode = null;
                    edgeEndInput = null;
                    edgeStartOutput = null;
                  } else {
                    edgeStartNode = node;
                    edgeEndInput = node.inputs[i].id;
                  }
                });
              },
              onDoubleTap: () {
                _showAttributeDialog(context, node.inputs[i]);
              },
              child: Tooltip(
                message: node.inputs[i].name,
                child: Icon(Icons.input,
                    size: 16,
                    color: edgeStartNode == node &&
                            edgeEndInput == node.inputs[i].id
                        ? Colors.white
                        : Colors.green),
              ),
            ),
          ));
        }

        for (int i = 0; i < node.outputs.length; i++) {
          // Distribute the outputs from radians pi/4 to 7pi/4 around the node.
          // If there is only one output, it should be at 0 or 2pi radians.
          final angle = (node.outputs.length == 1)
              ? 0
              : (pi / 4) + (i * (3 * pi / 2) / (node.outputs.length - 1));
          final iconOffset = Offset(
            node.position.dx + 30 * cos(angle),
            node.position.dy + 30 * sin(angle),
          );
          tooltips.add(Positioned(
            left: iconOffset.dx - 8,
            top: iconOffset.dy - 8,
            child: GestureDetector(
              onTapDown: (details) {
                setState(() {
                  edgeEndInput = null;
                  edgeStartNode = node;
                  edgeStartOutput = node.outputs[i].id;
                });
              },
              onDoubleTap: () {
                _showAttributeDialog(context, node.outputs[i]);
              },
              child: Tooltip(
                message: node.outputs[i].name,
                child: Icon(Icons.output,
                    size: 16,
                    color: edgeStartNode == node &&
                            edgeStartOutput == node.outputs[i].id
                        ? Colors.white
                        : Colors.green),
              ),
            ),
          ));
        }
      }
    }
    return tooltips;
  } // End of _buildTooltips

  void _showAttributeDialog(BuildContext context, Reference reference) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text('Edit ${reference.name} Attributes'),
          content: SingleChildScrollView(
            child: Column(
              children: [
                TextField(
                  controller: TextEditingController(text: reference.name),
                  decoration: InputDecoration(labelText: 'Name'),
                  onChanged: (value) {
                    reference.name = value;
                  },
                ),
                if (reference is Array) ...[
                  // Array specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.capacity.toString()),
                    decoration: InputDecoration(labelText: 'Capacity'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.capacity =
                          int.tryParse(value) ?? reference.capacity;
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.elemType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: arrayTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.elemType = value!;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.values.join(', ')),
                    decoration: InputDecoration(labelText: 'Values'),
                    keyboardType: TextInputType.text,
                    onChanged: (value) {
                      // Remove trailing commas and split
                      reference.values = value
                          .split(',')
                          .map((e) => e.trim())
                          .where((e) => e.isNotEmpty)
                          .toList();
                    },
                    onEditingComplete: () =>
                        _updateArrayCapacity(context, reference),
                    onTapOutside: (event) =>
                        _updateArrayCapacity(context, reference),
                  ),
                ],
                if (reference is Convolution) ...[
                  // Convolution specific attributes
                  TextField(
                    controller:
                        TextEditingController(text: reference.rows.toString()),
                    decoration: InputDecoration(labelText: 'Rows'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.rows = int.tryParse(value) ?? reference.rows;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.cols.toString()),
                    decoration: InputDecoration(labelText: 'Columns'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.cols = int.tryParse(value) ?? reference.cols;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.scale.toString()),
                    decoration: InputDecoration(labelText: 'Scale'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.scale =
                          (double.tryParse(value) ?? reference.scale).toInt();
                    },
                  ),
                ],
                if (reference is Img) ...[
                  // Img specific attributes
                  TextField(
                    controller:
                        TextEditingController(text: reference.width.toString()),
                    decoration: InputDecoration(labelText: 'Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.width = int.tryParse(value) ?? reference.width;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.height.toString()),
                    decoration: InputDecoration(labelText: 'Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.height =
                          int.tryParse(value) ?? reference.height;
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.format,
                    decoration: InputDecoration(labelText: 'Format'),
                    items: imageTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.format = value!;
                    },
                  ),
                ],
                if (reference is Lut) ...[
                  // Lut specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.capacity.toString()),
                    decoration: InputDecoration(labelText: 'Capacity'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.capacity =
                          int.tryParse(value) ?? reference.capacity;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.elemType),
                    decoration: InputDecoration(labelText: 'Element Type'),
                    onChanged: (value) {
                      reference.elemType = value;
                    },
                  ),
                ],
                if (reference is Matrix) ...[
                  // Matrix specific attributes
                  TextField(
                    controller:
                        TextEditingController(text: reference.rows.toString()),
                    decoration: InputDecoration(labelText: 'Rows'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.rows = int.tryParse(value) ?? reference.rows;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.cols.toString()),
                    decoration: InputDecoration(labelText: 'Columns'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.cols = int.tryParse(value) ?? reference.cols;
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.elemType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: numTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.elemType = value!;
                    },
                  ),
                ],
                if (reference is ObjectArray) ...[
                  // ObjectArray specific attributes
                  Builder(
                    builder: (context) {
                      final controller = TextEditingController(
                          text: reference.numObjects.toString());
                      return TextField(
                        controller: controller,
                        decoration:
                            InputDecoration(labelText: 'Number of Objects'),
                        keyboardType: TextInputType.number,
                        onEditingComplete: () {
                          final newValue = int.tryParse(controller.text) ?? 0;
                          if (newValue >= 0) {
                            reference.setNumObjects(newValue);
                            // Force rebuild of dialog to update the UI
                            Navigator.of(context).pop();
                            _showAttributeDialog(context, reference);
                          }
                        },
                        onTapOutside: (event) {
                          final newValue = int.tryParse(controller.text) ?? 0;
                          if (newValue >= 0) {
                            reference.setNumObjects(newValue);
                            // Force rebuild of dialog to update the UI
                            Navigator.of(context).pop();
                            _showAttributeDialog(context, reference);
                          }
                        },
                      );
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.elemType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: objectArrayTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      if (value != null) {
                        reference.elemType = value;
                        // Force rebuild of dialog to show new element type attributes
                        Navigator.of(context).pop();
                        _showAttributeDialog(context, reference);
                      }
                    },
                  ),
                  CheckboxListTile(
                    title: Text('Apply To All Objects'),
                    value: reference.applyToAll,
                    onChanged: (value) {
                      if (value != null) {
                        reference.applyToAll = value;
                        // Force rebuild of dialog to show/hide individual attributes
                        Navigator.of(context).pop();
                        _showAttributeDialog(context, reference);
                      }
                    },
                  ),
                  if (reference.applyToAll) ...[
                    // Show common attributes for all objects
                    ..._buildElementTypeAttributes(reference),
                  ] else if (reference.numObjects > 0) ...[
                    // Show individual attributes for each object
                    ...List.generate(reference.numObjects, (index) {
                      return ExpansionTile(
                        title: Text('Object ${index + 1}'),
                        children: _buildElementTypeAttributes(
                          reference,
                          objectIndex: index,
                        ),
                      );
                    }),
                  ],
                ],
                if (reference is Pyramid) ...[
                  // Pyramid specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.numLevels.toString()),
                    decoration: InputDecoration(labelText: 'Number of Levels'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.numLevels =
                          int.tryParse(value) ?? reference.numLevels;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.width.toString()),
                    decoration: InputDecoration(labelText: 'Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.width = int.tryParse(value) ?? reference.width;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.height.toString()),
                    decoration: InputDecoration(labelText: 'Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.height =
                          int.tryParse(value) ?? reference.height;
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.format,
                    decoration: InputDecoration(labelText: 'Format'),
                    items: imageTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.format = value!;
                    },
                  ),
                ],
                if (reference is Remap) ...[
                  // Remap specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.srcWidth.toString()),
                    decoration: InputDecoration(labelText: 'Source Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.srcWidth =
                          int.tryParse(value) ?? reference.srcWidth;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.srcHeight.toString()),
                    decoration: InputDecoration(labelText: 'Source Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.srcHeight =
                          int.tryParse(value) ?? reference.srcHeight;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.dstWidth.toString()),
                    decoration: InputDecoration(labelText: 'Destination Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.dstWidth =
                          int.tryParse(value) ?? reference.dstWidth;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(
                        text: reference.dstHeight.toString()),
                    decoration:
                        InputDecoration(labelText: 'Destination Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.dstHeight =
                          int.tryParse(value) ?? reference.dstHeight;
                    },
                  ),
                ],
                if (reference is Scalar) ...[
                  // Scalar specific attributes
                  DropdownButtonFormField<String>(
                    value: reference.elemType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: scalarTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.elemType = value!;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.value.toString()),
                    decoration: InputDecoration(labelText: 'Value'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.value =
                          double.tryParse(value) ?? reference.value;
                    },
                  ),
                ],
                if (reference is Tensor) ...[
                  // Tensor specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.numDims.toString()),
                    decoration:
                        InputDecoration(labelText: 'Number of Dimensions'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.numDims =
                          int.tryParse(value) ?? reference.numDims;
                    },
                  ),
                  TextField(
                    controller:
                        TextEditingController(text: reference.shape.toString()),
                    decoration: InputDecoration(labelText: 'Shape'),
                    onChanged: (value) {
                      reference.shape = value
                          .replaceAll(RegExp(r'[\[\]]'), '')
                          .split(',')
                          .map((e) => int.tryParse(e.trim()) ?? 0)
                          .toList();
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.elemType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: numTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.elemType = value!;
                    },
                  ),
                ],
                if (reference is Thrshld) ...[
                  // Threshold specific attributes
                  TextField(
                    controller:
                        TextEditingController(text: reference.thresType),
                    decoration: InputDecoration(labelText: 'Threshold Type'),
                    onChanged: (value) {
                      reference.thresType = value;
                    },
                  ),
                  DropdownButtonFormField<String>(
                    value: reference.dataType,
                    decoration: InputDecoration(labelText: 'Element Type'),
                    items: thresholdDataTypes.map((type) {
                      return DropdownMenuItem<String>(
                        value: type,
                        child: Text(type),
                      );
                    }).toList(),
                    onChanged: (value) {
                      reference.dataType = value!;
                    },
                  ),
                ],
                if (reference is UserDataObject) ...[
                  // UserDataObject specific attributes
                  TextField(
                    controller: TextEditingController(
                        text: reference.sizeInBytes.toString()),
                    decoration: InputDecoration(labelText: 'Size in Bytes'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.sizeInBytes =
                          int.tryParse(value) ?? reference.sizeInBytes;
                    },
                  ),
                ],
                // Add more fields for other Reference types as needed
              ],
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text('Close'),
            ),
          ],
        );
      },
    );
  } // End of _showAttributeDialog

  void _updateArrayCapacity(BuildContext context, Array reference) {
    // For strings, capacity is based on total character count
    // For other types, capacity is based on number of elements
    final newCapacity = reference.elemType == 'CHAR'
        ? reference.values.join(', ').length
        : reference.values.length;

    if (newCapacity != reference.capacity) {
      reference.capacity = newCapacity;
      // Force rebuild of the dialog to update the capacity field
      Navigator.of(context).pop();
      _showAttributeDialog(context, reference);
    }
  } // End of _updateArrayCapacity

  List<Widget> _buildElementTypeAttributes(ObjectArray reference,
      {int? objectIndex}) {
    // Get the appropriate attributes map based on whether we're dealing with individual objects
    Map<String, dynamic> attributes = objectIndex != null
        ? (reference.elementAttributes['object_$objectIndex']
                as Map<String, dynamic>? ??
            {})
        : reference.elementAttributes;

    // Helper function to get attribute value
    T? getAttribute<T>(String key) {
      return attributes[key] as T?;
    }

    // Helper function to set attribute value
    void setAttribute(String key, dynamic value) {
      if (objectIndex != null) {
        final objectKey = 'object_$objectIndex';
        if (!reference.elementAttributes.containsKey(objectKey)) {
          reference.elementAttributes[objectKey] = <String, dynamic>{};
        }

        final objectMap =
            reference.elementAttributes[objectKey] as Map<String, dynamic>;
        objectMap[key] = value;
      } else {
        reference.elementAttributes[key] = value;
      }
    }

    switch (reference.elemType) {
      case 'TENSOR':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('numDims')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Number of Dimensions'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('numDims', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<List<int>>('shape')?.toString() ?? '[]'),
            decoration: InputDecoration(labelText: 'Shape'),
            onChanged: (value) {
              setAttribute(
                  'shape',
                  value
                      .replaceAll(RegExp(r'[\[\]]'), '')
                      .split(',')
                      .map((e) => int.tryParse(e.trim()) ?? 0)
                      .toList());
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('elemType') ?? numTypes.first,
            decoration: InputDecoration(labelText: 'Element Type'),
            items: numTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('elemType', value);
              }
            },
          ),
        ];
      case 'IMAGE':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('width')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Width'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('width', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('height')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Height'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('height', int.tryParse(value) ?? 0);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('format') ?? imageTypes.first,
            decoration: InputDecoration(labelText: 'Format'),
            items: imageTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('format', value);
              }
            },
          ),
        ];
      case 'ARRAY':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('capacity')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Capacity'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('capacity', int.tryParse(value) ?? 0);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('elemType') ?? arrayTypes.first,
            decoration: InputDecoration(labelText: 'Element Type'),
            items: arrayTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('elemType', value);
              }
            },
          ),
        ];
      case 'MATRIX':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('rows')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Rows'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('rows', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('cols')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Columns'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('cols', int.tryParse(value) ?? 0);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('elemType') ?? numTypes.first,
            decoration: InputDecoration(labelText: 'Element Type'),
            items: numTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('elemType', value);
              }
            },
          ),
        ];
      case 'SCALAR':
        return [
          DropdownButtonFormField<String>(
            value: getAttribute<String>('elemType') ?? scalarTypes.first,
            decoration: InputDecoration(labelText: 'Element Type'),
            items: scalarTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('elemType', value);
              }
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<double>('value')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Value'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('value', double.tryParse(value) ?? 0.0);
            },
          ),
        ];
      case 'CONVOLUTION':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('rows')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Rows'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('rows', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('cols')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Columns'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('cols', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('scale')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Scale'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('scale', int.tryParse(value) ?? 0);
            },
          ),
        ];
      case 'PYRAMID':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('numLevels')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Number of Levels'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('numLevels', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('width')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Width'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('width', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('height')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Height'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('height', int.tryParse(value) ?? 0);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('format') ?? imageTypes.first,
            decoration: InputDecoration(labelText: 'Format'),
            items: imageTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('format', value);
              }
            },
          ),
        ];
      case 'REMAP':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('srcWidth')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Source Width'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('srcWidth', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('srcHeight')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Source Height'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('srcHeight', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('dstWidth')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Destination Width'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('dstWidth', int.tryParse(value) ?? 0);
            },
          ),
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('dstHeight')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Destination Height'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('dstHeight', int.tryParse(value) ?? 0);
            },
          ),
        ];
      case 'THRESHOLD':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<String>('thresType') ?? ''),
            decoration: InputDecoration(labelText: 'Threshold Type'),
            onChanged: (value) {
              setAttribute('thresType', value);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('dataType') ?? thresholdDataTypes.first,
            decoration: InputDecoration(labelText: 'Data Type'),
            items: thresholdDataTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('dataType', value);
              }
            },
          ),
        ];
      case 'LUT':
        return [
          TextField(
            controller: TextEditingController(
                text: getAttribute<int>('capacity')?.toString() ?? '0'),
            decoration: InputDecoration(labelText: 'Capacity'),
            keyboardType: TextInputType.number,
            onChanged: (value) {
              setAttribute('capacity', int.tryParse(value) ?? 0);
            },
          ),
          DropdownButtonFormField<String>(
            value: getAttribute<String>('elemType') ?? numTypes.first,
            decoration: InputDecoration(labelText: 'Element Type'),
            items: numTypes.map((type) {
              return DropdownMenuItem<String>(
                value: type,
                child: Text(type),
              );
            }).toList(),
            onChanged: (value) {
              if (value != null) {
                setAttribute('elemType', value);
              }
            },
          ),
        ];
      default:
        return [];
    }
  }
} // End of GraphEditorState class

class GraphListPanel extends StatelessWidget {
  const GraphListPanel({
    super.key,
    required this.graphs,
    required this.selectedGraphRow,
    required this.onAddGraph,
    required this.onSelectGraph,
  });

  final List<Graph> graphs;
  final int? selectedGraphRow;
  final VoidCallback onAddGraph;
  final Function(int) onSelectGraph;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 80,
      color: Colors.grey[900],
      child: Row(
        children: [
          IconButton(
            icon: Icon(Icons.add),
            tooltip: 'Add Graph',
            onPressed: onAddGraph,
          ),
          Expanded(
            child: ListView.builder(
              scrollDirection: Axis.horizontal,
              itemCount: graphs.length,
              itemBuilder: (context, index) {
                return Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 8.0),
                  child: GestureDetector(
                    onTap: () => onSelectGraph(index),
                    child: Chip(
                      label: Text('Graph ${index + 1}'),
                      backgroundColor: selectedGraphRow == index
                          ? Colors.blue
                          : Colors.grey[700],
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
} // End of GraphListPanel

class NodeAttributesPanel extends StatelessWidget {
  final Graph? graph;
  final Node? selectedNode;
  final List<Target> supportedTargets;
  final TextEditingController nameController;
  final FocusNode nameFocusNode;
  final Function(String) onNameChanged;
  final Function(String) onTargetChanged;
  final Function(String) onKernelChanged;
  final VoidCallback? onNameEditComplete;

  const NodeAttributesPanel({
    super.key,
    required this.graph,
    required this.selectedNode,
    required this.supportedTargets,
    required this.nameController,
    required this.nameFocusNode,
    required this.onNameChanged,
    required this.onTargetChanged,
    required this.onKernelChanged,
    this.onNameEditComplete,
  });

  @override
  Widget build(BuildContext context) {
    return AnimatedContainer(
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
                  controller:
                      TextEditingController(text: selectedNode!.id.toString()),
                  decoration: InputDecoration(
                    labelText: 'ID',
                  ),
                  // Make ID field read-only
                  enabled: false,
                ),
                SizedBox(height: 8.0),
                TextField(
                  controller: nameController,
                  focusNode: nameFocusNode,
                  decoration: InputDecoration(
                    labelText: 'Name',
                  ),
                  onChanged: onNameChanged,
                  onEditingComplete: () {
                    FocusScope.of(context).unfocus(); // Dismiss the keyboard
                    if (onNameEditComplete != null) onNameEditComplete!();
                  },
                ),
                SizedBox(height: 8.0),
                DropdownButtonFormField<String>(
                  isExpanded: true,
                  value: selectedNode!.target,
                  decoration: InputDecoration(
                    labelText:
                        Text('Target', overflow: TextOverflow.ellipsis).data,
                    isDense: true,
                  ),
                  items: supportedTargets
                      .map((target) => DropdownMenuItem<String>(
                            alignment: Alignment.centerLeft,
                            value: target.name,
                            child: Text(
                              target.name,
                              overflow: TextOverflow.ellipsis,
                            ),
                          ))
                      .toList(),
                  onChanged: (newValue) {
                    onTargetChanged(newValue!);
                  },
                ),
                SizedBox(height: 8.0),
                DropdownButtonFormField<String>(
                  isExpanded: true,
                  value: selectedNode!.kernel,
                  decoration: InputDecoration(
                    labelText:
                        Text('Kernel', overflow: TextOverflow.ellipsis).data,
                    isDense: true,
                  ),
                  items: supportedTargets
                      .firstWhere(
                        (target) => target.name == selectedNode!.target,
                        orElse: () => supportedTargets.first,
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
                    onKernelChanged(newValue!);
                  },
                ),
                SizedBox(height: 8.0),
                _buildDependenciesSection(
                  title: 'Upstream Dependencies',
                  dependencies: graph!.getUpstreamDependencies(selectedNode!),
                ),
                SizedBox(height: 8.0),
                _buildDependenciesSection(
                  title: 'Downstream Dependencies',
                  dependencies: graph!.getDownstreamDependencies(selectedNode!),
                ),
                SizedBox(height: 8.0),
                Text(
                  'Inputs',
                ),
                Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: supportedTargets
                      .firstWhere(
                        (target) => target.name == selectedNode!.target,
                        orElse: () => supportedTargets.first,
                      )
                      .kernels
                      .firstWhere(
                        (kernel) => kernel.name == selectedNode!.kernel,
                        orElse: () => supportedTargets.first.kernels.first,
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
                  children: supportedTargets
                      .firstWhere(
                        (target) => target.name == selectedNode!.target,
                        orElse: () => supportedTargets.first,
                      )
                      .kernels
                      .firstWhere(
                        (kernel) => kernel.name == selectedNode!.kernel,
                        orElse: () => supportedTargets.first.kernels.first,
                      )
                      .outputs
                      .map((output) => Text(output))
                      .toList(),
                ),
                // Add more attributes as needed
              ],
            )
          : Container(),
    );
  }

  Widget _buildDependenciesSection({
    required String title,
    required List<String> dependencies,
  }) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          title,
          // style: TextStyle(fontWeight: FontWeight.bold),
        ),
        ...dependencies.map((dep) => TextField(
              controller: TextEditingController(text: dep),
              enabled: false, // Make dependencies read-only
            )),
      ],
    );
  }
}
