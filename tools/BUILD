cc_binary(
    name = "run_xml_graph",
    srcs = ["run_xml_graph.cpp"],
    deps = [
        "//:corevx",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        # "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
)