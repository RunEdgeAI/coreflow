
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
        "//tests/xyz:imported_xyz",
    ],
    linkopts = [
        # This tells the linker to search for shared libraries relative to the binary's location
        "-Wl,-rpath,@executable_path",
    ],
    data = [
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//tests/raw:raw_data",
        "//tests/xyz:imported_xyz",
    ],
)