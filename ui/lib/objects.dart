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
    if (name == ('TYPE_ARRAY')) {
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

  Map<String, dynamic> toJson() => {
    'id': id,
    'name': name,
    'type': type,
    'linkId': linkId,
  };

  static Reference fromJson(Map<String, dynamic> json) {
    final type = json['type'] ?? '';
    switch (type) {
      case 'Array':
        return Array.fromJson(json);
      case 'Convolution':
        return Convolution.fromJson(json);
      case 'Image':
        return Img.fromJson(json);
      case 'Lut':
        return Lut.fromJson(json);
      case 'Matrix':
        return Matrix.fromJson(json);
      case 'ObjectArray':
        return ObjectArray.fromJson(json);
      case 'Pyramid':
        return Pyramid.fromJson(json);
      case 'Remap':
        return Remap.fromJson(json);
      case 'Scalar':
        return Scalar.fromJson(json);
      case 'Tensor':
        return Tensor.fromJson(json);
      case 'Threshold':
        return Thrshld.fromJson(json);
      case 'UserDataObject':
        return UserDataObject.fromJson(json);
      case 'Node':
        return Node.fromJson(json);
      default:
        return Reference(
          id: json['id'],
          name: json['name'] ?? '',
          type: type,
          linkId: json['linkId'] ?? -1,
        );
    }
  }
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'position': {'dx': position.dx, 'dy': position.dy},
    'kernel': kernel,
    'target': target,
    'inputs': inputs.map((e) => e.toJson()).toList(),
    'outputs': outputs.map((e) => e.toJson()).toList(),
  };

  static Node fromJson(Map<String, dynamic> json) {
    return Node(
      id: json['id'],
      name: json['name'] ?? '',
      position: Offset(
        (json['position']['dx'] as num).toDouble(),
        (json['position']['dy'] as num).toDouble(),
      ),
      kernel: json['kernel'] ?? '',
      target: json['target'] ?? '',
      inputs: (json['inputs'] as List<dynamic>? ?? [])
          .map((e) => Reference.fromJson(e as Map<String, dynamic>))
          .toList(),
      outputs: (json['outputs'] as List<dynamic>? ?? [])
          .map((e) => Reference.fromJson(e as Map<String, dynamic>))
          .toList(),
    );
  }
}

class Graph extends Reference {
  List<Node> nodes;
  List<Edge> edges;
  Graph({required super.id, super.type = 'Graph', required this.nodes, required this.edges});

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

  Map<String, dynamic> toJson() => {
    'id': id,
    'type': type,
    'nodes': nodes.map((n) => n.toJson()).toList(),
    'edges': edges.map((e) => e.toJson()).toList(),
  };

  static Graph fromJson(Map<String, dynamic> json) {
    final nodes = (json['nodes'] as List<dynamic>? ?? [])
        .map((e) => Node.fromJson(e as Map<String, dynamic>))
        .toList();
    // Build a map for node lookup by id
    final nodeMap = {for (var n in nodes) n.id: n};
    final edges = (json['edges'] as List<dynamic>? ?? [])
        .map((e) => Edge.fromJson(e as Map<String, dynamic>, nodeMap))
        .toList();
    return Graph(
      id: json['id'],
      nodes: nodes,
      edges: edges,
    );
  }
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'capacity': capacity,
    'elemType': elemType,
    'values': values,
  };

  static Array fromJson(Map<String, dynamic> json) => Array(
    id: json['id'],
    name: json['name'] ?? '',
    capacity: json['capacity'] ?? 0,
    elemType: json['elemType'] ?? '',
    values: json['values'] ?? [],
  );
}

class Convolution extends Matrix {
  int scale;
  Convolution({
    required super.id,
    required super.name,
    required super.rows,
    required super.cols,
    this.scale = 1,
    super.elemType = 'TYPE_INT16',
    super.type = 'Convolution',
  });

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'scale': scale,
  };

  static Convolution fromJson(Map<String, dynamic> json) => Convolution(
    id: json['id'],
    name: json['name'] ?? '',
    rows: json['rows'] ?? 0,
    cols: json['cols'] ?? 0,
    scale: json['scale'] ?? 1,
    elemType: json['elemType'] ?? 'TYPE_INT16',
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'width': width,
    'height': height,
    'format': format,
  };

  static Img fromJson(Map<String, dynamic> json) => Img(
    id: json['id'],
    name: json['name'] ?? '',
    width: json['width'] ?? 0,
    height: json['height'] ?? 0,
    format: json['format'] ?? '',
  );
}

class Lut extends Array {
  Lut({
    required super.id,
    required super.name,
    required super.capacity,
    super.elemType = 'TYPE_UINT8',
    super.type = 'Lut',
  });

  @override
  Map<String, dynamic> toJson() => super.toJson();

  static Lut fromJson(Map<String, dynamic> json) => Lut(
    id: json['id'],
    name: json['name'] ?? '',
    capacity: json['capacity'] ?? 0,
    elemType: json['elemType'] ?? 'TYPE_UINT8',
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'rows': rows,
    'cols': cols,
    'elemType': elemType,
  };

  static Matrix fromJson(Map<String, dynamic> json) => Matrix(
    id: json['id'],
    name: json['name'] ?? '',
    rows: json['rows'] ?? 0,
    cols: json['cols'] ?? 0,
    elemType: json['elemType'] ?? '',
  );
}

class ObjectArray extends Reference {
  int numObjects;
  String elemType;
  Map<String, dynamic> elementAttributes;
  bool applyToAll;

  ObjectArray({
    required super.id,
    required super.name,
    super.type = 'ObjectArray',
    required this.numObjects,
    required this.elemType,
    Map<String, dynamic>? elementAttributes,
    this.applyToAll = true,
  }) : elementAttributes = elementAttributes ?? {};

  // Helper method to get element attribute with type safety
  T? getElementAttribute<T>(String key) {
    return elementAttributes[key] as T?;
  }

  // Helper method to set element attribute
  void setElementAttribute(String key, dynamic value) {
    elementAttributes[key] = value;
  }

  void setNumObjects(int value) {
    if (value < 0) {
      throw ArgumentError('Number of objects cannot be negative');
    }
    // Clean up individual object attributes for removed objects
    if (value < numObjects) {
      for (int i = value; i < numObjects; i++) {
        elementAttributes.remove('object_$i');
      }
    }
    numObjects = value;
  }

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'numObjects': numObjects,
    'elemType': elemType,
    'elementAttributes': elementAttributes,
    'applyToAll': applyToAll,
  };

  static ObjectArray fromJson(Map<String, dynamic> json) => ObjectArray(
    id: json['id'],
    name: json['name'] ?? '',
    numObjects: json['numObjects'] ?? 0,
    elemType: json['elemType'] ?? '',
    elementAttributes: Map<String, dynamic>.from(json['elementAttributes'] ?? {}),
    applyToAll: json['applyToAll'] ?? true,
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'width': width,
    'height': height,
    'format': format,
    'numLevels': numLevels,
    // 'levels': levels, // Not serializing Image objects for now
  };

  static Pyramid fromJson(Map<String, dynamic> json) => Pyramid(
    id: json['id'],
    name: json['name'] ?? '',
    width: json['width'] ?? 0,
    height: json['height'] ?? 0,
    format: json['format'] ?? '',
    numLevels: json['numLevels'] ?? 0,
    // levels: [], // Not deserializing Image objects for now
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'srcWidth': srcWidth,
    'srcHeight': srcHeight,
    'dstWidth': dstWidth,
    'dstHeight': dstHeight,
  };

  static Remap fromJson(Map<String, dynamic> json) => Remap(
    id: json['id'],
    name: json['name'] ?? '',
    srcWidth: json['srcWidth'] ?? 0,
    srcHeight: json['srcHeight'] ?? 0,
    dstWidth: json['dstWidth'] ?? 0,
    dstHeight: json['dstHeight'] ?? 0,
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'elemType': elemType,
    'value': value,
  };

  static Scalar fromJson(Map<String, dynamic> json) => Scalar(
    id: json['id'],
    name: json['name'] ?? '',
    elemType: json['elemType'] ?? '',
    value: (json['value'] as num?)?.toDouble() ?? 0.0,
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'numDims': numDims,
    'shape': shape,
    'elemType': elemType,
  };

  static Tensor fromJson(Map<String, dynamic> json) => Tensor(
    id: json['id'],
    name: json['name'] ?? '',
    numDims: json['numDims'] ?? 0,
    shape: (json['shape'] as List<dynamic>? ?? []).map((e) => e as int).toList(),
    elemType: json['elemType'] ?? '',
  );
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

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'thresType': thresType,
    'binary': binary,
    'lower': lower,
    'upper': upper,
    'trueVal': trueVal,
    'falseVal': falseVal,
    'dataType': dataType,
  };

  static Thrshld fromJson(Map<String, dynamic> json) => Thrshld(
    id: json['id'],
    name: json['name'] ?? '',
    thresType: json['thresType'] ?? '',
    binary: json['binary'] ?? 0,
    lower: json['lower'] ?? 0,
    upper: json['upper'] ?? 0,
    trueVal: json['trueVal'] ?? 0,
    falseVal: json['falseVal'] ?? 0,
    dataType: json['dataType'] ?? '',
  );
}

class UserDataObject extends Reference {
  int sizeInBytes;
  UserDataObject({
    required super.id,
    required super.name,
    super.type = 'UserDataObject',
    required this.sizeInBytes,
  });

  @override
  Map<String, dynamic> toJson() => {
    ...super.toJson(),
    'sizeInBytes': sizeInBytes,
  };

  static UserDataObject fromJson(Map<String, dynamic> json) => UserDataObject(
    id: json['id'],
    name: json['name'] ?? '',
    sizeInBytes: json['sizeInBytes'] ?? 0,
  );
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

  Map<String, dynamic> toJson() => {
    'source': source.id,
    'target': target.id,
    'srcId': srcId,
    'tgtId': tgtId,
  };

  static Edge fromJson(Map<String, dynamic> json, Map<int, Node> nodeMap) {
    return Edge(
      source: nodeMap[json['source']]!,
      target: nodeMap[json['target']]!,
      srcId: json['srcId'],
      tgtId: json['tgtId'],
    );
  }
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
