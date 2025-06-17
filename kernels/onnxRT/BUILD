
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
        "//third_party:onnxruntime",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"]
)