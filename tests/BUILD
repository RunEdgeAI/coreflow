
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
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        # "//targets/opencl:imported_openvx_opencl",
        "//tests/xyz:imported_xyz",
    ],
    linkopts = [
        # This tells the linker to search for shared libraries relative to the binary's location
        "-Wl,-rpath,@executable_path",
    ],
    data = [
        "//tests/raw:raw_data",
    ],
)