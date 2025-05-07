
cc_library(
    name = "torch_kernels",
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
        "//third_party:executorch",
    ],
    alwayslink = True,
    visibility = ["//visibility:public"]
)