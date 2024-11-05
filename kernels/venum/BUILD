

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
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
    ],
    visibility = ["//visibility:public"]
)