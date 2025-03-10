import 'package:flutter/material.dart';

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
  Graph({
    required super.id,
    required this.nodes,
    required this.edges
  });
}

class Array extends Reference {
  int capacity;
  String elemType;
  Array({
    required super.id,
    required super.name,
    required this.capacity,
    required this.elemType,
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
  });
}

class Image extends Reference {
  int width;
  int height;
  String format;
  Image({
    required super.id,
    required super.name,
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
  });
}

class Matrix extends Reference {
  int rows;
  int cols;
  String elemType;
  Matrix({
    required super.id,
    required super.name,
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
  String elemType;
  Pyramid({
    required super.id,
    required super.name,
    required this.width,
    required this.height,
    required this.format,
    required this.numLevels,
    required this.elemType,
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
    required this.numDims,
    required this.shape,
    required this.elemType,
  });
}

class Threshold extends Reference {
  String thresType;
  int binary;
  int lower;
  int upper;
  int trueVal;
  int falseVal;
  Threshold({
    required super.id,
    required super.name,
    required this.thresType,
    this.binary = 0,
    this.lower = 0,
    this.upper = 0,
    this.trueVal = 0,
    this.falseVal = 0,
  });
}

class UserDataObject extends Reference {
  int sizeInBytes;
  UserDataObject({
    required super.id,
    required super.name,
    required this.sizeInBytes,
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