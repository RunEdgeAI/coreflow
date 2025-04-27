# cc_test(
#     name = "test_ort",
#     srcs = [
#         "test_ort.cpp"
#     ],
#     includes = [
#         "include",
#         "framework/include"
#     ],
#     deps = [
#         "//:corevx",
#         "@googletest//:gtest_main",
#         "//targets/c_model:imported_openvx_c_model",
#         "//targets/debug:imported_openvx_debug",
#         "//targets/extras:imported_openvx_extras",
#         "//targets/opencl:imported_openvx_opencl",
#         "//targets/onnxRT:imported_openvx_onnxRT",
#     ],
#     linkopts = select({
#         "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
#         "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
#         "//conditions:default": [],
#     }),
#     data = [
#         "//tests/raw:models",
#     ],
#     size = "small"
# )

cc_test(
    name = "test_tflite",
    srcs = [
        "test_tflite.cpp"
    ],
    includes = [
        "include",
        "framework/include"
    ],
    deps = [
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/liteRT:imported_openvx_liteRT",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    data = [
        "//tests/raw:models",
    ],
    size = "small"
)