import 'dart:io';
import 'objects.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:xml/xml.dart' as xml;

class XmlExport {
  const XmlExport(
      {required this.graphs, required this.graphIndex, required this.refCount});

  final List<Graph> graphs;
  final int graphIndex;
  final int refCount;

  // Map of format codes to their standardized form
  static const Map<String, String> _formatMap = {
    'U1': 'U001',
    'U8': 'U008',
    'U16': 'U016',
    'S16': 'S016',
    'U32': 'U032',
    'S32': 'S032',
    'RGB': 'RGB2',
  };

  void export(BuildContext context) {
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
    } else {
      // Show a message if there are no graphs to export
      showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text("No Graphs Defined!"),
          content: Text("There are no graphs to export."),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("Close"),
            ),
          ],
        ),
      );
    }
  }

  void _addReferenceElement(xml.XmlBuilder builder, Reference ref) {
    if (ref.linkId != -1) {
      // Reuse the reference by pointing to the linked reference ID
      return;
    }

    if (ref is ObjectArray) {
      builder.element('object_array', nest: () {
        builder.attribute('reference', ref.id);
        builder.attribute('count', ref.numObjects);
        builder.attribute('objType', ref.elemType);

        // Only export one child object if applyToAll is true
        final numChildren =
            (ref.numObjects > 0 && ref.applyToAll) ? 1 : ref.numObjects;

        for (int i = 0; i < numChildren; i++) {
          final objectAttrs = ref.applyToAll
              ? ref.elementAttributes
              : ref.elementAttributes['object_$i'] as Map<String, dynamic>?;

          if (objectAttrs == null) continue;

          switch (ref.elemType) {
            // Handle specific reference types
            case 'TENSOR':
              builder.element('tensor', nest: () {
                builder.attribute('numDims', objectAttrs['numDims'] ?? 0);
                builder.attribute('elemType',
                    "VX_TYPE_${objectAttrs['elemType'] ?? 'UINT8'}");
                if (objectAttrs['shape'] != null) {
                  builder.element('shape', nest: () {
                    builder.text((objectAttrs['shape'] as List).join(', '));
                  });
                }
              });
              break;
            case 'IMAGE':
              builder.element('image', nest: () {
                builder.attribute('width', objectAttrs['width'] ?? 0);
                builder.attribute('height', objectAttrs['height'] ?? 0);
                final format = objectAttrs['format'] ?? 'U8';
                // Use the format map or keep the original format
                final formattedFormat = _formatMap[format] ?? format;
                builder.attribute('format', formattedFormat);
              });
              break;
            case 'ARRAY':
              builder.element('array', attributes: {
                'capacity': objectAttrs['capacity']?.toString() ?? '0',
                'elemType': "VX_TYPE_${objectAttrs['elemType'] ?? 'UINT8'}",
              });
              break;
            case 'MATRIX':
              builder.element('matrix', attributes: {
                'rows': objectAttrs['rows']?.toString() ?? '0',
                'columns': objectAttrs['cols']?.toString() ?? '0',
                'elemType': "VX_TYPE_${objectAttrs['elemType'] ?? 'UINT8'}",
              });
              break;
            case 'SCALAR':
              builder.element('scalar', attributes: {
                'elemType': "VX_TYPE_${objectAttrs['elemType'] ?? 'UINT8'}",
              }, nest: () {
                builder.element(
                    objectAttrs['elemType']?.toString().toLowerCase() ??
                        'uint8',
                    nest: objectAttrs['value']?.toString() ?? '0');
              });
              break;
            case 'CONVOLUTION':
              builder.element('convolution', attributes: {
                'rows': objectAttrs['rows']?.toString() ?? '0',
                'columns': objectAttrs['cols']?.toString() ?? '0',
                'scale': objectAttrs['scale']?.toString() ?? '0',
              });
              break;
            case 'PYRAMID':
              builder.element('pyramid', attributes: {
                'width': objectAttrs['width']?.toString() ?? '0',
                'height': objectAttrs['height']?.toString() ?? '0',
                'format':
                    objectAttrs['format']?.toString() ?? 'VX_DF_IMAGE_VIRT',
                'levels': objectAttrs['numLevels']?.toString() ?? '0',
              });
              break;
            case 'REMAP':
              builder.element('remap', attributes: {
                'src_width': objectAttrs['srcWidth']?.toString() ?? '0',
                'src_height': objectAttrs['srcHeight']?.toString() ?? '0',
                'dst_width': objectAttrs['dstWidth']?.toString() ?? '0',
                'dst_height': objectAttrs['dstHeight']?.toString() ?? '0',
              });
              break;
            case 'THRESHOLD':
              builder.element('threshold', attributes: {
                'reference': '0',
              }, nest: () {
                if (objectAttrs['thresType'] == 'TYPE_BINARY') {
                  builder.element('binary', nest: '0');
                } else if (objectAttrs['thresType'] == 'TYPE_RANGE') {
                  builder.element('range', attributes: {
                    'lower': '0',
                    'upper': '0',
                  });
                }
              });
              break;
            case 'LUT':
              builder.element('lut', attributes: {
                'capacity': objectAttrs['capacity']?.toString() ?? '0',
                'elemType': "VX_TYPE_${objectAttrs['elemType'] ?? 'UINT8'}",
              });
              break;
          }
        }
      });
    } else if (ref is Img) {
      builder.element('image', attributes: {
        'reference': ref.id.toString(),
        'width': ref.width.toString(),
        'height': ref.height.toString(),
        'format': ref.format,
      });
    } else if (ref is Scalar) {
      builder.element('scalar', attributes: {
        'reference': ref.id.toString(),
        'elemType': "VX_TYPE_${ref.elemType}",
      }, nest: () {
        builder.element(ref.elemType.toLowerCase(), nest: ref.value.toString());
      });
    } else if (ref is Array) {
      builder.element('array',
          attributes: {
            'reference': ref.id.toString(),
            'capacity': ref.capacity.toString(),
            'elemType': "VX_TYPE_${ref.elemType}",
          },
          nest: ref.values.isEmpty
              ? null
              : () {
                  final type = ref.elemType.toUpperCase();

                  if (type == 'CHAR') {
                    // Export as a single <char>...</char> element
                    builder.element('char', nest: ref.values.join());
                  } else if (type == 'RECTANGLE' && ref.values.length == 4) {
                    // Export as <rectangle><start_x>...</start_x>...</rectangle>
                    builder.element('rectangle', nest: () {
                      builder.element('start_x', nest: ref.values[0]);
                      builder.element('start_y', nest: ref.values[1]);
                      builder.element('end_x', nest: ref.values[2]);
                      builder.element('end_y', nest: ref.values[3]);
                    });
                  } else if (type == 'COORDINATES2D' &&
                      ref.values.length == 2) {
                    builder.element('coordinates2d', nest: () {
                      builder.element('x', nest: ref.values[0]);
                      builder.element('y', nest: ref.values[1]);
                    });
                  } else if (type == 'COORDINATES3D' &&
                      ref.values.length == 3) {
                    builder.element('coordinates3d', nest: () {
                      builder.element('x', nest: ref.values[0]);
                      builder.element('y', nest: ref.values[1]);
                      builder.element('z', nest: ref.values[2]);
                    });
                  } else if (type.startsWith('FLOAT') ||
                      type.startsWith('INT') ||
                      type.startsWith('UINT')) {
                    // Export each value as <float32>...</float32> or <int16>...</int16> etc.
                    final tag = type.toLowerCase();
                    for (final v in ref.values) {
                      // Validate/parse as number
                      final parsed = num.tryParse(v);
                      builder.element(tag, nest: parsed?.toString() ?? v);
                    }
                  } else {
                    // Default: export each value as <value>...</value>
                    for (final v in ref.values) {
                      builder.element('value', nest: v);
                    }
                  }
                });
    } else if (ref is Convolution) {
      builder.element('convolution', attributes: {
        'reference': ref.id.toString(),
        'rows': ref.rows.toString(),
        'columns': ref.cols.toString(),
        'scale': ref.scale.toString(),
      });
    } else if (ref is Matrix) {
      builder.element('matrix', attributes: {
        'reference': ref.id.toString(),
        'rows': ref.rows.toString(),
        'columns': ref.cols.toString(),
        'elemType': "VX_TYPE_${ref.elemType}",
      });
    } else if (ref is Pyramid) {
      builder.element('pyramid', attributes: {
        'reference': ref.id.toString(),
        'width': ref.width.toString(),
        'height': ref.height.toString(),
        'format': ref.format,
        'levels': ref.numLevels.toString(),
      });
    } else if (ref is Thrshld) {
      builder.element('threshold', attributes: {
        'reference': ref.id.toString(),
      }, nest: () {
        if (ref.thresType == 'TYPE_BINARY') {
          builder.element('binary', nest: ref.binary.toString());
        } else if (ref.thresType == 'TYPE_RANGE') {
          builder.element('range', attributes: {
            'lower': ref.lower.toString(),
            'upper': ref.upper.toString(),
          });
        }
      });
    } else if (ref is Remap) {
      builder.element('remap', attributes: {
        'reference': ref.id.toString(),
        'src_width': ref.srcWidth.toString(),
        'src_height': ref.srcHeight.toString(),
        'dst_width': ref.dstWidth.toString(),
        'dst_height': ref.dstHeight.toString(),
      });
    } else {
      // Default case for unknown reference types
      builder.element('reference', attributes: {
        'reference': ref.id.toString(),
        'type': ref.type,
      });
    }
  }

  String _exportXML(Graph graph, int refCount) {
    final builder = xml.XmlBuilder();
    final Set<String> targets = _getTargets(graph);

    builder.processing('xml', 'version="1.0" encoding="utf-8"');

    builder.element('openvx', attributes: {
      'xmlns:xsi': 'https://www.w3.org/TR/xmlschema-1',
      'xmlns': 'https://www.khronos.org/registry/vx/schema',
      'xsi:schemaLocation':
          'https://registry.khronos.org/OpenVX/schema/openvx-1-1.xsd',
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
      targets.add('openvx-${node.target}');
    }

    return targets;
  } // End of _getTargets
} // End of XmlExport

class DotExport {
  const DotExport({
    required this.graphs,
    required this.graphIndex,
  });

  final List<Graph> graphs;
  final int graphIndex;

  void export(BuildContext context) {
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
    } else {
      // Show a message if there are no graphs to export
      showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text("No Graphs Defined!"),
          content: Text("There are no graphs to export."),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("Close"),
            ),
          ],
        ),
      );
    }
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
          dot.writeln(
              '  D$referenceId [shape=box label="${_formatReferenceLabel(input)}"];');
          addedReferences.add(referenceId);
        }
      }
      for (var output in node.outputs) {
        final referenceId = output.id;
        if (!addedReferences.contains(referenceId)) {
          dot.writeln(
              '  D$referenceId [shape=box label="${_formatReferenceLabel(output)}"];');
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
      dot.writeln(
          '  D${targetReferenceId != -1 ? targetReferenceId : edge.tgtId} -> N${edge.target.id};');
    }

    // End the Graph
    dot.writeln('}');
    return dot.toString();
  }
} // End of DotExport
