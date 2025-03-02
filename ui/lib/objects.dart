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