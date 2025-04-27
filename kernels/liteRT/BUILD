
cc_library(
    name = "liteRT_kernels",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "*.hpp",
    ]),
    includes = [
        ".",
        "//framework/include",
    ],
    deps = [
        "//:corevx",
        "//third_party:tflite",
        "//third_party:tflite-hdrs",
    ],
    visibility = ["//visibility:public"]
)