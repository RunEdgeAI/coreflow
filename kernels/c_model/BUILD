
cc_library(
    name = "c_model_kernels",
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
        "//kernels/debug:debug_kernels",
        "//kernels/utils"
    ],
    visibility = ["//visibility:public"]
)