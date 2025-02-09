
cc_library(
    name = "ort_kernels",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "*.hpp",
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