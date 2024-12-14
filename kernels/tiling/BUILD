
cc_library(
    name = "tiling_kernels",
    srcs = glob([
        "*.cpp"
    ]),
    hdrs = glob([
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include"
    ],
    deps = [
        "//:corevx",
    ],
    target_compatible_with = [
        "@platforms//cpu:aarch64",
    ],
    visibility = ["//visibility:public"]
)