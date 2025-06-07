import 'package:flutter/material.dart';
import 'package:ui/utils.dart';

List<String> refTypes = [
  "ARRAY",
  "CONVOLUTION",
  "IMAGE",
  "LUT",
  "MATRIX",
  "OBJECT_ARRAY",
  "PYRAMID",
  "REMAP",
  "SCALAR",
  "TENSOR",
  "THRESHOLD",
  "USER_DATA_OBJECT",
];

List<String> objectArrayTypes = refTypes
    .where((type) => type != 'ARRAY' && type != 'OBJECT_ARRAY')
    .toList();

List<String> imageTypes = [
  "VIRT",
  "RGB",
  "RGBX",
  "NV12",
  "NV21",
  "UYVY",
  "YUYV",
  "IYUV",
  "YUV4",
  "U1",
  "U8",
  "U16",
  "S16",
  "U32",
  "S32",
];

List<String> numTypes = [
  "INT8",
  "UINT8",
  "INT16",
  "UINT16",
  "INT32",
  "UINT32",
  "INT64",
  "UINT64",
  "FLOAT16",
  "FLOAT32",
  "FLOAT64",
];

List<String> scalarTypes = numTypes +
    [
      "CHAR",
      "DF_IMAGE",
      "ENUM",
      "SIZE",
      "BOOL",
    ];

List<String> arrayTypes = scalarTypes +
    [
      "RECTANGLE",
      "KEYPOINT",
      "COORDINATES2D",
      "COORDINATES3D",
      "COORDINATES2DF",
    ];

List<String> thresholdTypes = [
  "TYPE_BINARY",
  "TYPE_RANGE",
];

List<String> thresholdDataTypes = scalarTypes
    .where((type) =>
        type != 'CHAR' &&
        type != 'DF_IMAGE' &&
        type != 'ENUM' &&
        type != 'SIZE' &&
        type != 'FLOAT16' &&
        type != 'FLOAT32' &&
        type != 'FLOAT64')
    .toList();

class Reference {
  final int id;
  String name;
  String type;
  int linkId;
  Reference({
    required this.id,
    this.name = '',
    this.type = '',
    this.linkId = -1,
  });

  static Reference createReference(String name, int refCount) {
    // Logic to determine the type of Reference to create
    if (name == ('VX_TYPE_ARRAY')) {
      return Array(
          id: refCount, name: name, capacity: 0, elemType: arrayTypes.first);
    } else if (name.contains('CONVOLUTION')) {
      return Convolution(id: refCount, name: name, rows: 0, cols: 0, scale: 1);
    } else if (name.contains('IMAGE')) {
      return Img(
          id: refCount,
          name: name,
          width: 0,
          height: 0,
          format: imageTypes.first);
    } else if (name.contains('LUT')) {
      return Lut(id: refCount, name: name, capacity: 0);
    } else if (name.contains('MATRIX')) {
      return Matrix(
          id: refCount, name: name, rows: 0, cols: 0, elemType: numTypes.first);
    } else if (name.contains('OBJECT_ARRAY')) {
      return ObjectArray(
          id: refCount,
          name: name,
          numObjects: 0,
          elemType: objectArrayTypes.first);
    } else if (name.contains('PYRAMID')) {
      return Pyramid(
          id: refCount,
          name: name,
          numLevels: 0,
          width: 0,
          height: 0,
          format: imageTypes.first);
    } else if (name.contains('REMAP')) {
      return Remap(
          id: refCount,
          name: name,
          srcWidth: 0,
          srcHeight: 0,
          dstWidth: 0,
          dstHeight: 0);
    } else if (name.contains('SCALAR')) {
      return Scalar(
          id: refCount, name: name, elemType: scalarTypes.first, value: 0.0);
    } else if (name.contains('TENSOR')) {
      return Tensor(
          id: refCount,
          name: name,
          shape: [],
          numDims: 0,
          elemType: numTypes.first);
    } else if (name.contains('THRESHOLD')) {
      return Thrshld(
          id: refCount,
          name: name,
          thresType: thresholdTypes.first,
          dataType: thresholdDataTypes.first);
    } else if (name.contains('USER_DATA_OBJECT')) {
      return UserDataObject(id: refCount, name: name, sizeInBytes: 0);
    }

    // Add more conditions for other Reference types as needed
    return Reference(id: refCount, name: name);
  } // End of _createReference
}

class Node extends Reference {
  Offset position;
  String kernel;
  String target;
  List<Reference> inputs;
  List<Reference> outputs;
  Node({
    required super.id,
    required super.name,
    super.type = 'Node',
    required this.position,
    required this.kernel,
    required this.target,
    this.inputs = const [],
    this.outputs = const [],
  });
}

class Graph extends Reference {
  List<Node> nodes;
  List<Edge> edges;
  Graph(
      {required super.id,
      super.type = 'Graph',
      required this.nodes,
      required this.edges});

  Node? findNodeAt(Offset position) {
    for (var node in nodes.reversed) {
      if ((node.position - position).distance < 25) {
        return node;
      }
    }
    return null;
  } // End of _findNodeAt

  Edge? findEdgeAt(Offset position) {
    for (var edge in edges.reversed) {
      if (Utils.isPointNearEdge(
          position, edge.source.position, edge.target.position)) {
        return edge;
      }
    }
    return null;
  } // End of _findEdgeAt

  List<String> getUpstreamDependencies(Node node) {
    return edges
        .where((edge) => edge.target == node)
        .map((edge) => edge.source.name)
        .toList();
  } // End of _getUpstreamDependencies

  List<String> getDownstreamDependencies(Node node) {
    return edges
        .where((edge) => edge.source == node)
        .map((edge) => edge.target.name)
        .toList();
  } // End of _getDownstreamDependencies
}

class Array extends Reference {
  int capacity;
  String elemType;
  List<dynamic> values;
  Array({
    required super.id,
    required super.name,
    super.type = 'Array',
    required this.capacity,
    required this.elemType,
    this.values = const [],
  });
}

class Convolution extends Matrix {
  int scale;
  Convolution({
    required super.id,
    required super.name,
    required super.rows,
    required super.cols,
    this.scale = 1,
    super.elemType = 'VX_TYPE_INT16',
    super.type = 'Convolution',
  });
}

class Img extends Reference {
  int width;
  int height;
  String format;
  Img({
    required super.id,
    required super.name,
    super.type = 'Image',
    required this.width,
    required this.height,
    required this.format,
  });
}

class Lut extends Array {
  Lut({
    required super.id,
    required super.name,
    required super.capacity,
    super.elemType = 'VX_TYPE_UINT8',
    super.type = 'Lut',
  });
}

class Matrix extends Reference {
  int rows;
  int cols;
  String elemType;
  Matrix({
    required super.id,
    required super.name,
    super.type = 'Matrix',
    required this.rows,
    required this.cols,
    required this.elemType,
  });
}

class ObjectArray extends Reference {
  int numObjects;
  String elemType;
  ObjectArray({
    required super.id,
    required super.name,
    super.type = 'ObjectArray',
    required this.numObjects,
    required this.elemType,
  });
}

class Pyramid extends Reference {
  int width;
  int height;
  String format;
  List<Image> levels;
  int numLevels;
  Pyramid({
    required super.id,
    required super.name,
    super.type = 'Pyramid',
    required this.width,
    required this.height,
    required this.format,
    required this.numLevels,
    this.levels = const [],
  });
}

class Remap extends Reference {
  int srcWidth;
  int srcHeight;
  int dstWidth;
  int dstHeight;
  Remap({
    required super.id,
    required super.name,
    super.type = 'Remap',
    required this.srcWidth,
    required this.srcHeight,
    required this.dstWidth,
    required this.dstHeight,
  });
}

class Scalar extends Reference {
  String elemType;
  double value;
  Scalar({
    required super.id,
    required super.name,
    super.type = 'Scalar',
    required this.elemType,
    required this.value,
  });
}

class Tensor extends Reference {
  int numDims;
  List<int> shape;
  String elemType;
  Tensor({
    required super.id,
    required super.name,
    super.type = 'Tensor',
    required this.numDims,
    required this.shape,
    required this.elemType,
  });
}

class Thrshld extends Reference {
  String thresType;
  int binary;
  int lower;
  int upper;
  int trueVal;
  int falseVal;
  String dataType;
  Thrshld({
    required super.id,
    required super.name,
    super.type = 'Threshold',
    required this.thresType,
    this.binary = 0,
    this.lower = 0,
    this.upper = 0,
    this.trueVal = 0,
    this.falseVal = 0,
    required this.dataType,
  });
}

class UserDataObject extends Reference {
  int sizeInBytes;
  UserDataObject({
    required super.id,
    required super.name,
    super.type = 'UserDataObject',
    required this.sizeInBytes,
  });
}

class Edge {
  final Node source;
  final Node target;
  final int srcId;
  final int tgtId;
  Edge({
    required this.source,
    required this.target,
    required this.srcId,
    required this.tgtId,
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
