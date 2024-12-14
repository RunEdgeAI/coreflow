

cc_library(
    name = "venum_kernels",
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
        "//kernels/utils",
    ],
    target_compatible_with = [
        "@platforms//cpu:aarch64",
    ],
    visibility = ["//visibility:public"]
)