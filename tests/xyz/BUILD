

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
    visibility = ["//visibility:public"]
)