cc_test(
    name = "test_advanced_api",
    srcs = [
        "test_advanced_api.cpp"
    ],
    deps = [
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    size = "small"
)

cc_test(
    name = "test_aiserver",
    srcs = [
        "test_aiserver.cpp"
    ],
    deps = [
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    size = "small"
)

cc_test(
    name="test_ort",
    srcs=[
        "test_ort.cpp"
    ],
    deps=[
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
    ],
    linkopts=select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    data=[
        "//tests/raw:models",
    ],
    size="small"
)

cc_test(
    name="test_pipelining",
    srcs=[
        "test_pipelining.cpp"
    ],
    deps=[
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",

    ],
    linkopts=select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    data=[
        "//tests/raw:models",
    ],
    size="small"
)

cc_test(
    name = "test_tflite",
    srcs = [
        "test_tflite.cpp"
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

cc_test(
    name = "test_torch",
    srcs = [
        "test_torch.cpp"
    ],
    deps = [
        "//:corevx",
        "@googletest//:gtest_main",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/executorch:imported_openvx_torch",
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