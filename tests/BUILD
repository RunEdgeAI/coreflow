
cc_binary(
    name = "vx_test",
    srcs = [
        "vx_tests.cpp",
    ],
    includes = [
        "//framework/include",
        "//include"
    ],
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
        "//tests/xyz:imported_xyz",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-Wl,-rpath,$ORIGIN"],
        "@platforms//os:macos": ["-Wl,-rpath,@executable_path"],
        "//conditions:default": [],
    }),
    data = [
        "//tests/raw:raw_data",
    ],
)