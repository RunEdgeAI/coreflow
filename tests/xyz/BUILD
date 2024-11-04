

cc_library(
    name = "xyz-static",
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
    name = "xyz",
    deps = [
        ":xyz-static"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_xyz",
    shared_library = ":xyz",
    visibility = ["//visibility:public"]
)