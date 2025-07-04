
cc_binary(
    name = "canny-edge-detector",
    srcs = [
        "src/canny.cpp",
    ],
    deps = [
        "//:corevx",
        "@opencv",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
        "//third_party:opencv-prebuilt",
    ],
    visibility = ["//visibility:public"],
)