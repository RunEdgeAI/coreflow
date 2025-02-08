cc_test(
    name = "test_ort",
    srcs = [
        "test_ort.cpp"
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
        "//targets/onnxRT:imported_openvx_onnxRT",
    ],
    linkopts = [
        "-Wl,-rpath,@executable_path",
    ],
    data = [
        "//tests/raw:models",
    ],
    size = "small"
)