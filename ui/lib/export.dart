import 'dart:io';
import 'objects.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:xml/xml.dart' as xml;

class XmlExport extends StatelessWidget {
  const XmlExport({
    super.key,
    required this.graphs,
    required this.graphIndex,
    required this.refCount
  });

  final List<Graph> graphs;
  final int graphIndex;
  final int refCount;

  @override
  Widget build(BuildContext context) {
    return IconButton(
      icon: Icon(Icons.code_off),
      tooltip: 'Export XML',
      onPressed: () {
        if (graphs.isNotEmpty) {
          final graph = graphs[graphIndex];
          // Export the selected graph
          final xml = _exportXML(graph, refCount);
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
    );
  }

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

  String _exportXML(Graph graph, int refCount) {
    final builder = xml.XmlBuilder();
    final Set<String> targets = _getTargets(graph);

    builder.processing('xml', 'version="1.0" encoding="utf-8"');

    builder.element('openvx', namespaces: {
      '': 'https://www.khronos.org/registry/vx/schema',
      'xsi': 'https://www.w3.org/TR/xmlschema-1'
    }, attributes: {
      'xsi:schemaLocation': 'https://registry.khronos.org/OpenVX/schema/openvx-1-1.xsd',
      'references': refCount.toString()
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
} // End of XmlExport

class DotExport extends StatelessWidget {
  const DotExport({
    super.key,
    required this.graphs,
    required this.graphIndex,
  });

  final List<Graph> graphs;
  final int graphIndex;

  @override
  Widget build(BuildContext context) {
    return IconButton(
      icon: Icon(Icons.code),
      tooltip: 'Export DOT',
      onPressed: () {
        // Check if there are any graphs available
        if (graphs.isNotEmpty) {
          final graph = graphs[graphIndex];
          // Export the selected graph
          final dot = _exportDOT(graph);
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
    );
  }


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
  }
} // End of DotExport