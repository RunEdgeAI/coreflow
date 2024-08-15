
cc_library(
    name = "openvx-debug",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/c_model",
        "//kernels/c_model/debug",
        "//kernels/utils"
    ],
    deps = [
        "//:corevx",
        "//kernels/c_model/debug:debug_kernels"
    ],
    visibility = ["//visibility:public"]
)