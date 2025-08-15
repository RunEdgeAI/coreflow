
package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "hello_world",
    srcs = ["hello_world.cpp"],
    deps = [
        "//:corevx",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
    ],
)

cc_binary(
    name = "blur_pipeline",
    srcs = ["blur_pipeline.cpp"],
    deps = [
        "//:corevx",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
    ],
)

cc_binary(
    name = "custom_code",
    srcs = ["custom_code.cpp"],
    deps = [
        "//:corevx",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
    ],
)
