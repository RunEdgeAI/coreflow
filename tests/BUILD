
cc_library(
    name = "xyz",
    srcs = [
        "vx_xyz_lib.cpp",
        "vx_xyz_module.cpp",
    ],
    includes = [
        ".",
        "//include"
    ],
    deps = [
        "//:corevx",
    ],
)

cc_shared_library(
    name = "xyz-shared",
    shared_lib_name = "libxyz.dylib",
    deps = [
        ":xyz"
    ]
)

cc_import(
    name = "imported_xyz",
    shared_library = ":xyz-shared",  # Correctly import the shared library
)

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
        ":imported_xyz",
    ],
    linkopts = [
        "-Wl,-rpath,@executable_path",  # This tells the linker to search for shared libraries relative to the binary's location
    ],
    data = [
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//tests/raw:raw_data",
        ":imported_xyz",
    ],
)