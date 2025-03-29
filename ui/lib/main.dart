import 'dart:io';
import 'dart:math';
import 'objects.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:xml/xml.dart' as xml;
import 'painter.dart';

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

  @override
  void initState() {
    super.initState();
    _focusNode.requestFocus();
    _loadSupportedXml();
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
  } // End of _loadSupportedXml

  void _addGraph() {
    setState(() {
      graphs.add(Graph(id: _refCount++, nodes: [], edges: []));
      selectedGraphIndex = graphs.length - 1;
    });
    _deselectAll();
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
  } // End of _deleteGraph

  void _addNode(Graph graph, Offset position, Size panelSize) {
    // Assuming the radius of the node is 25
    final nodeRadius = 25.0;
    final clampedX = position.dx.clamp(nodeRadius, panelSize.width - nodeRadius);
    final clampedY = position.dy.clamp(nodeRadius, panelSize.height - nodeRadius);
    final clampedPosition = Offset(clampedX, clampedY);

    setState(() {
      final newNode = Node(
        id: _refCount++,
        name: 'Node $nextId',
        position: clampedPosition,
        target: _supported.first.name,
        kernel : _supported.first.kernels.first.name,
      );
      _updateNodeIO(newNode, newNode.kernel);
      nextId++;
      graph.nodes.add(newNode);
    });
    _deselectAll();
  } // End of _addNode

  void _addEdge(Graph graph, Node source, Node target, int srcId, int tgtId) {
    // Check if the source and target nodes are the same
    if (source == target) {
      return;
    }

    // Check if an edge already exists between the same pair of nodes
    bool edgeExists = graph.edges.any((edge) =>
      (edge.source == source && edge.target == target && edge.srcId == srcId && edge.tgtId == tgtId));

    if (!edgeExists) {
      setState(() {
        final index = target.inputs.indexWhere((input) => input.id == tgtId);
        if (index >= 0 && index < target.inputs.length) {
          // If an input was tapped, update its linkId to be source.id.
          // target.inputs[index] = source.outputs.firstWhere((output) => output.id == srcId);
          target.inputs[index].linkId = srcId;
          // Create a new edge
          final newEdge = Edge(source: source, target: target, srcId: srcId, tgtId: tgtId);
          graph.edges.add(newEdge);
        }
      });
      // Deselect selected node and any selected edge after creating an edge
      _deselectAll();
    }
  } // End of _addEdge

  void _deselectAll() {
    setState(() {
      selectedNode = null;
      selectedEdge = null;
      selectedGraphRow = null;
    });
  } // End of _deselectAll

  void _deleteSelected(Graph graph) {
    setState(() {
      if (selectedNode != null) {
        graph.edges.removeWhere((edge) =>
          edge.source == selectedNode || edge.target == selectedNode);
        _refCount -= selectedNode!.inputs.length + selectedNode!.outputs.length + 1;
        graph.nodes.remove(selectedNode);
        selectedNode = null;
      } else if (selectedEdge != null) {
        graph.edges.remove(selectedEdge);
      }
      _deselectAll();
    });
  } // End of _deleteSelected

  String _formatReferenceLabel(Reference reference) {
    if (reference is Img) {
      return 'Image\\n${reference.width}x${reference.height}\\n${reference.format}';
    } else if (reference is Array) {
      return 'Array\\n${reference.elemType}\\nItems: ${reference.capacity}';
    } else if (reference is Scalar) {
      return 'Scalar\\n${reference.elemType}\\nValue: ${reference.value}';
    } else if (reference is Convolution) {
      return 'Convolution\\n${reference.rows}x${reference.cols}\\nScale: ${reference.scale}';
    } else if (reference is Matrix) {
      return 'Matrix\\n${reference.rows}x${reference.cols}\\n${reference.elemType}';
    } else if (reference is Pyramid) {
      return 'Pyramid\\n${reference.numLevels} Levels\\n${reference.width}x${reference.height}\\n${reference.format}';
    } else if (reference is Thrshld) {
      return 'Threshold\\n${reference.thresType}\\n${reference.dataType}';
    } else if (reference is Remap) {
      return 'Remap\\nSrc: ${reference.srcWidth}x${reference.srcHeight}\\nDst: ${reference.dstWidth}x${reference.dstHeight}';
    } else if (reference is Lut) {
      return 'LUT\\n${reference.elemType}\\nCapacity: ${reference.capacity}';
    } else if (reference is Tensor) {
      return 'Tensor\\nDims: ${reference.numDims}\\nShape: ${reference.shape.join(", ")}\\n${reference.elemType}';
    } else if (reference is UserDataObject) {
      return 'UserDataObject\\nSize: ${reference.sizeInBytes} bytes';
    } else {
      return '${reference.type}\\nID: ${reference.id}';
    }
  }
  String _exportDOT(Graph graph) {
    StringBuffer dot = StringBuffer();

    // Add the Graph header
    dot.writeln('digraph G {');
    dot.writeln('  size=4;');
    dot.writeln('  rank=LR;');
    dot.writeln('  node [shape=oval style=filled fillcolor=red fontsize=27];');

    // Add the nodes
    for (var node in graph.nodes) {
      dot.writeln('  node${node.id} [label="${node.name}"];');
    }

    // Add data objects
    final Set<int> addedReferences = {};
    for (var node in graph.nodes) {
      for (var input in node.inputs) {
        final referenceId = input.linkId != -1 ? input.linkId : input.id;
        if (!addedReferences.contains(referenceId)) {
          dot.writeln('  D$referenceId [shape=box label="${_formatReferenceLabel(input)}"];');
          addedReferences.add(referenceId);
        }
      }
      for (var output in node.outputs) {
        final referenceId = output.id;
        if (!addedReferences.contains(referenceId)) {
          dot.writeln('  D$referenceId [shape=box label="${_formatReferenceLabel(output)}"];');
          addedReferences.add(referenceId);
        }
      }
    }

    // Add the edges
    for (var edge in graph.edges) {
      // Use linkId for the source reference if it exists
      final sourceReferenceId = edge.srcId;
      final targetReferenceId = graph.nodes
          .expand((node) => node.inputs)
          .firstWhere((input) => input.id == edge.tgtId)
          .linkId;

      // Edge from source node's output to the data object
      dot.writeln('  N${edge.source.id} -> D$sourceReferenceId;');

      // Edge from the data object to the target node's input
      dot.writeln('  D${targetReferenceId != -1 ? targetReferenceId : edge.tgtId} -> N${edge.target.id};');
    }

    // End the Graph
    dot.writeln('}');
    return dot.toString();
  } // End of _exportDOT

  void _addReferenceElement(xml.XmlBuilder builder, Reference reference) {
    if (reference.linkId != -1) {
      // Reuse the reference by pointing to the linked reference ID
      return;
    }

    // Handle specific reference types
    if (reference is Img) {
      builder.element('image', attributes: {
        'reference': reference.id.toString(),
        'width': reference.width.toString(),
        'height': reference.height.toString(),
        'format': reference.format,
      });
    } else if (reference is Scalar) {
      builder.element('scalar', attributes: {
        'reference': reference.id.toString(),
        'elemType': reference.elemType,
      }, nest: () {
        builder.element(reference.elemType.toLowerCase(), nest: reference.value.toString());
      });
    } else if (reference is Array) {
      builder.element('array', attributes: {
        'reference': reference.id.toString(),
        'capacity': reference.capacity.toString(),
        'elemType': reference.elemType,
      });
    } else if (reference is Convolution) {
      builder.element('convolution', attributes: {
        'reference': reference.id.toString(),
        'rows': reference.rows.toString(),
        'columns': reference.cols.toString(),
        'scale': reference.scale.toString(),
      });
    } else if (reference is Matrix) {
      builder.element('matrix', attributes: {
        'reference': reference.id.toString(),
        'rows': reference.rows.toString(),
        'columns': reference.cols.toString(),
        'elemType': reference.elemType,
      });
    } else if (reference is Pyramid) {
      builder.element('pyramid', attributes: {
        'reference': reference.id.toString(),
        'width': reference.width.toString(),
        'height': reference.height.toString(),
        'format': reference.format,
        'levels': reference.numLevels.toString(),
      });
    } else if (reference is Thrshld) {
      builder.element('threshold', attributes: {
        'reference': reference.id.toString(),
      }, nest: () {
        if (reference.thresType == 'TYPE_BINARY') {
          builder.element('binary', nest: reference.binary.toString());
        } else if (reference.thresType == 'TYPE_RANGE') {
          builder.element('range', attributes: {
            'lower': reference.lower.toString(),
            'upper': reference.upper.toString(),
          });
        }
      });
    } else if (reference is Remap) {
      builder.element('remap', attributes: {
        'reference': reference.id.toString(),
        'src_width': reference.srcWidth.toString(),
        'src_height': reference.srcHeight.toString(),
        'dst_width': reference.dstWidth.toString(),
        'dst_height': reference.dstHeight.toString(),
      });
    } else {
      // Default case for unknown reference types
      builder.element('reference', attributes: {
        'reference': reference.id.toString(),
        'type': reference.type,
      });
    }
  }

  String _exportXML(Graph graph) {
    final builder = xml.XmlBuilder();
    final Set<String> targets = _getTargets(graph);

    builder.processing('xml', 'version="1.0" encoding="utf-8"');

    builder.element('openvx', namespaces: {
      '': 'https://www.khronos.org/registry/vx/schema',
      'xsi': 'https://www.w3.org/TR/xmlschema-1'
    }, attributes: {
      'xsi:schemaLocation': 'https://registry.khronos.org/OpenVX/schema/openvx-1-1.xsd',
      'references': _refCount.toString()
    }, nest: () {
      // Add library entries for each target
      for (var target in targets) {
        builder.element('library', nest: target);
      }

      // Add input and output references
      for (var node in graph.nodes) {
        for (var input in node.inputs) {
          _addReferenceElement(builder, input);
        }
        for (var output in node.outputs) {
          _addReferenceElement(builder, output);
        }
      }

      // Describe graph
      builder.element('graph', attributes: {
        'reference': graph.id.toString(),
        'name': 'GRAPH${graph.id}',
      }, nest: () {
        // Add nodes and kernels
        for (var node in graph.nodes) {
          builder.element('node', attributes: {
            'reference': node.id.toString(),
          }, nest: () {
            builder.element('kernel', nest: node.kernel);

            // Add input parameters
            for (int i = 0; i < node.inputs.length; i++) {
              builder.element('parameter', attributes: {
                'index': i.toString(),
                'reference': node.inputs[i].linkId != -1
                  ? node.inputs[i].linkId.toString()
                  : node.inputs[i].id.toString(),
              });
            }

            // Add output parameters
            for (int i = 0; i < node.outputs.length; i++) {
              builder.element('parameter', attributes: {
                'index': (node.inputs.length + i).toString(),
                'reference': node.outputs[i].id.toString(),
              });
            }
          });
        }

        // Add graph input and output parameters
        for (var edge in graph.edges) {
          builder.element('parameter', attributes: {
            'node': edge.source.id.toString(),
            'parameter': edge.srcId.toString(),
            'index': edge.tgtId.toString(),
          });
        }
      });
    });

    final document = builder.buildDocument();
    return document.toXmlString(pretty: true);
  } // End of _exportXML

  Set<String> _getTargets(Graph graph) {
    final Set<String> targets = {};

    for (var node in graph.nodes) {
      targets.add('openvx_${node.target}');
    }

    return targets;
  } // End of _getTargets

  List<String> _getUpstreamDependencies(Node node) {
    final graph = graphs[selectedGraphIndex];
    return graph.edges
        .where((edge) => edge.target == node)
        .map((edge) => edge.source.name)
        .toList();
  } // End of _getUpstreamDependencies

  List<String> _getDownstreamDependencies(Node node) {
    final graph = graphs[selectedGraphIndex];
    return graph.edges
        .where((edge) => edge.source == node)
        .map((edge) => edge.target.name)
        .toList();
  } // End of _getDownstreamDependencies

  Node? _findNodeAt(Graph graph, Offset position) {
    for (var node in graph.nodes.reversed) {
      if ((node.position - position).distance < 25) {
        return node;
      }
    }
    return null;
  } // End of _findNodeAt

  Edge? _findEdgeAt(Graph graph, Offset position) {
    for (var edge in graph.edges.reversed) {
      if (_isPointNearEdge(position, edge.source.position, edge.target.position)) {
        return edge;
      }
    }
    return null;
  } // End of _findEdgeAt

  bool _isPointNearEdge(Offset point, Offset start, Offset end) {
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

  void _updateNameController() {
    if (selectedNode != null && _nameController.text != selectedNode!.name) {
      _nameController.text = selectedNode!.name;
    }
  } // End of _updateNameController

  void _updateNodeIO(Node node, String kernelName) {
    final target = _supported.firstWhere((t) => t.name == node.target);
    final kernel = target.kernels.firstWhere((k) => k.name == kernelName);

    setState(() {
      node.inputs = kernel.inputs.map((input) => _createReference(input)).toList();
      node.outputs = kernel.outputs.map((output) => _createReference(output)).toList();
      _buildTooltips();
    });
  } // End of _updateNodeIO

  Reference _createReference(String name) {
    // Logic to determine the type of Reference to create
    if (name == ('VX_TYPE_ARRAY')) {
      return Array(id: _refCount++, name: name, capacity: 0, elemType: arrayTypes.first);
    } else if (name.contains('CONVOLUTION')) {
      return Convolution(id: _refCount++, name: name, rows: 0, cols: 0, scale: 1);
    } else if (name.contains('IMAGE')) {
      return Img(id: _refCount++, name: name, width: 0, height: 0, format: imageTypes.first);
    } else if (name.contains('LUT')) {
      return Lut(id: _refCount++, name: name, capacity: 0);
    } else if (name.contains('MATRIX')) {
      return Matrix(id: _refCount++, name: name, rows: 0, cols: 0, elemType: numTypes.first);
    } else if (name.contains('OBJECT_ARRAY')) {
      return ObjectArray(id: _refCount++, name: name, numObjects: 0, elemType: objectArrayTypes.first);
    } else if (name.contains('PYRAMID')) {
      return Pyramid(id: _refCount++, name: name, numLevels: 0, width: 0, height: 0, format: imageTypes.first);
    } else if (name.contains('REMAP')) {
      return Remap(id: _refCount++, name: name, srcWidth: 0, srcHeight: 0, dstWidth: 0, dstHeight: 0);
    } else if (name.contains('SCALAR')) {
      return Scalar(id: _refCount++, name: name, elemType: scalarTypes.first, value: 0.0);
    } else if (name.contains('TENSOR')) {
      return Tensor(id: _refCount++, name: name, shape: [], numDims: 0, elemType: numTypes.first);
    } else if (name.contains('THRESHOLD')) {
      return Thrshld(id: _refCount++, name: name, thresType: thresholdTypes.first, dataType: thresholdDataTypes.first);
    } else if (name.contains('USER_DATA_OBJECT')) {
      return UserDataObject(id: _refCount++, name: name, sizeInBytes: 0);
    }

    // Add more conditions for other Reference types as needed
    return Reference(id: _refCount++, name: name);
  } // End of _createReference

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
                        onPressed: () async {
                          final result = await FilePicker.platform.saveFile(
                            dialogTitle: 'Save DOT File',
                            fileName: 'graph.dot',
                          );

                          if (result != null) {
                            final file = File(result);
                            await file.writeAsString(dot);

                            // Show a confirmation message
                            if (context.mounted) {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(content: Text('DOT file saved to $result')),
                              );
                            }
                          }

                          if (context.mounted) {
                            Navigator.of(context).pop(); // Close the dialog
                          }
                        },
                        child: Text("Save"),
                      ),
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
                        onPressed: () async {
                          // Use FilePicker to save the XML file
                          final result = await FilePicker.platform.saveFile(
                            dialogTitle: 'Save XML File',
                            fileName: 'graph.xml',
                          );

                          if (result != null) {
                            final file = File(result);
                            await file.writeAsString(xml);

                            // Show a confirmation message
                            if (context.mounted) {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(content: Text('XML file saved to $result')),
                              );
                            }
                          }

                          // Close the dialog
                          if (context.mounted) {
                            Navigator.of(context).pop();
                          }
                        },
                        child: Text("Save"),
                      ),
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
      body: MouseRegion(
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
        child: KeyboardListener(
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
                          child: Stack(
                            children: [
                              graphs.isNotEmpty
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
                                        // _addEdge(graph, selectedNode!, tappedNode);
                                        // Deselect the selected node
                                        selectedNode = null;
                                      }
                                    } else if (tappedEdge != null) {
                                      if (selectedEdge == tappedEdge) {
                                        // Deselect the tapped edge if it is already selected
                                        selectedEdge = null;
                                      } else {
                                        // Deselect the selected node
                                        selectedNode = null;
                                        // Select the tapped edge
                                        selectedEdge = tappedEdge;
                                      }
                                    } else {
                                      _addNode(graph, details.localPosition, constraints.biggest);
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
                                    mousePosition = details.localPosition;
                                    if (draggingNode != null) {
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
                                    }
                                  });
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
                                    edgeStartNode = null;
                                    edgeStartOutput = null;
                                    mousePosition = null;
                                  });
                                },
                                child: CustomPaint(
                                  painter: graphs.isNotEmpty
                                      ? GraphPainter(
                                          graphs[selectedGraphIndex].nodes,
                                          graphs[selectedGraphIndex].edges,
                                          selectedNode,
                                          selectedEdge,
                                          mousePosition,
                                        )
                                      : null,
                                  child: Container(),
                                  ),
                              )
                                : Center(child: Text('No graphs available')),
                              ..._buildTooltips(),
                            ],
                          ),
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
                            value: selectedNode!.target,
                            decoration: InputDecoration(
                              labelText: Text(
                                          'Target',
                                          overflow: TextOverflow.ellipsis).data,
                              isDense: true,
                            ),
                            items: _supported
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
                              setState(() {
                                selectedNode!.target = newValue!;
                                // Set kernel to the first available value if target changes.
                                final target = _supported.firstWhere((t) => t.name == newValue);
                                if (target.kernels.isNotEmpty) {
                                  selectedNode!.kernel = target.kernels.first.name;
                                  _updateNodeIO(selectedNode!, selectedNode!.kernel);
                                }
                              });
                            },
                          ),
                          SizedBox(height: 8.0),
                          DropdownButtonFormField<String>(
                            isExpanded: true,
                            value: selectedNode!.kernel,
                            decoration: InputDecoration(
                              labelText: Text(
                                          'Kernel',
                                          overflow: TextOverflow.ellipsis).data,
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
                                _updateNodeIO(selectedNode!, newValue);
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
                    _addEdge(graph, edgeStartNode!, node, edgeStartOutput!, node.inputs[i].id);
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
                child: Icon(
                  Icons.input,
                  size: 16,
                  color: edgeStartNode == node && edgeEndInput == node.inputs[i].id
                    ? Colors.white
                    : Colors.green
                  ),
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
                child: Icon(
                  Icons.output,
                  size: 16,
                  color: edgeStartNode == node && edgeStartOutput == node.outputs[i].id
                    ? Colors.white
                    : Colors.green
                  ),
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
                if (reference is Array) ...[ // Array specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.capacity.toString()),
                    decoration: InputDecoration(labelText: 'Capacity'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.capacity = int.tryParse(value) ?? reference.capacity;
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
                ],
                if (reference is Convolution) ...[ // Convolution specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.rows.toString()),
                    decoration: InputDecoration(labelText: 'Rows'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.rows = int.tryParse(value) ?? reference.rows;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.cols.toString()),
                    decoration: InputDecoration(labelText: 'Columns'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.cols = int.tryParse(value) ?? reference.cols;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.scale.toString()),
                    decoration: InputDecoration(labelText: 'Scale'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.scale = (double.tryParse(value) ?? reference.scale).toInt();
                    },
                  ),
                ],
                if (reference is Img) ...[ // Img specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.width.toString()),
                    decoration: InputDecoration(labelText: 'Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.width = int.tryParse(value) ?? reference.width;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.height.toString()),
                    decoration: InputDecoration(labelText: 'Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.height = int.tryParse(value) ?? reference.height;
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
                if (reference is Lut) ...[ // Lut specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.capacity.toString()),
                    decoration: InputDecoration(labelText: 'Capacity'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.capacity = int.tryParse(value) ?? reference.capacity;
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
                if (reference is Matrix) ...[ // Matrix specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.rows.toString()),
                    decoration: InputDecoration(labelText: 'Rows'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.rows = int.tryParse(value) ?? reference.rows;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.cols.toString()),
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
                if (reference is ObjectArray) ...[ // ObjectArray specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.numObjects.toString()),
                    decoration: InputDecoration(labelText: 'Number of Objects'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.numObjects = int.tryParse(value) ?? reference.numObjects;
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
                      reference.elemType = value!;
                    },
                  ),
                ],
                if (reference is Pyramid) ...[ // Pyramid specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.numLevels.toString()),
                    decoration: InputDecoration(labelText: 'Number of Levels'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.numLevels = int.tryParse(value) ?? reference.numLevels;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.width.toString()),
                    decoration: InputDecoration(labelText: 'Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.width = int.tryParse(value) ?? reference.width;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.height.toString()),
                    decoration: InputDecoration(labelText: 'Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.height = int.tryParse(value) ?? reference.height;
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
                if (reference is Remap) ...[ // Remap specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.srcWidth.toString()),
                    decoration: InputDecoration(labelText: 'Source Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.srcWidth = int.tryParse(value) ?? reference.srcWidth;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.srcHeight.toString()),
                    decoration: InputDecoration(labelText: 'Source Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.srcHeight = int.tryParse(value) ?? reference.srcHeight;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.dstWidth.toString()),
                    decoration: InputDecoration(labelText: 'Destination Width'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.dstWidth = int.tryParse(value) ?? reference.dstWidth;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.dstHeight.toString()),
                    decoration: InputDecoration(labelText: 'Destination Height'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.dstHeight = int.tryParse(value) ?? reference.dstHeight;
                    },
                  ),
                ],
                if (reference is Scalar) ...[ // Scalar specific attributes
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
                    controller: TextEditingController(text: reference.value.toString()),
                    decoration: InputDecoration(labelText: 'Value'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.value = double.tryParse(value) ?? reference.value;
                    },
                  ),
                ],
                if (reference is Tensor) ...[ // Tensor specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.numDims.toString()),
                    decoration: InputDecoration(labelText: 'Number of Dimensions'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.numDims = int.tryParse(value) ?? reference.numDims;
                    },
                  ),
                  TextField(
                    controller: TextEditingController(text: reference.shape.toString()),
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
                if (reference is Thrshld) ...[ // Threshold specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.thresType),
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
                if (reference is UserDataObject) ...[ // UserDataObject specific attributes
                  TextField(
                    controller: TextEditingController(text: reference.sizeInBytes.toString()),
                    decoration: InputDecoration(labelText: 'Size in Bytes'),
                    keyboardType: TextInputType.number,
                    onChanged: (value) {
                      reference.sizeInBytes = int.tryParse(value) ?? reference.sizeInBytes;
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
} // End of GraphEditorState
// End of GraphEditorApp