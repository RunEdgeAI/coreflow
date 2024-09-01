
cc_library(
    name = "debug_kernels",
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
        "//kernels/utils"
    ],
    visibility = ["//visibility:public"]
)