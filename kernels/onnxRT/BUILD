
cc_library(
    name = "ort_kernels",
    srcs = glob([
        "*.cpp",
        "*.hpp"
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
        "@onnx_runtime//:onnxruntime_lib",
    ],
    visibility = ["//visibility:public"]
)