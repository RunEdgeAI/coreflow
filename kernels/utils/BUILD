
cc_library(
    name = "utils",
    srcs = glob([
        "*.cpp",
        "half/*.cpp"
    ]),
    hdrs = glob([
        "*.h",
        "half/*.hpp"
    ]),
    includes = [
        ".",
        "//framework/include"
    ],
    deps = [
        "//:corevx"
    ],
    visibility = ["//visibility:public"]
)