import 'package:flutter/material.dart';

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