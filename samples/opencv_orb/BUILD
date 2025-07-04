cc_binary(
    name = "opencv_orb",
    srcs = [
        "orb.cpp",
    ],
    deps = [
        "//:corevx",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/opencv:imported_openvx_opencv",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
    ],
    visibility = ["//visibility:public"],
)