
cc_library(
    name = "liteRT",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
    ],
    deps = [
        "//:corevx",
        "//kernels/liteRT:liteRT_kernels",
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-liteRT",
    deps = [":liteRT"],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_liteRT",
    shared_library = ":openvx-liteRT",
    visibility = ["//visibility:public"]
)