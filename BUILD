# Build library for the main project
cc_library(
    name = "corevx",
    srcs = glob([
        "framework/src/*.cpp"
    ]),
    hdrs = glob([
        "include/**",
        "framework/include/*.h",
    ]),
    includes = [
        "include",
        "framework/include"
    ],
    linkopts = [
        "-lxml2"
    ],
    visibility = ["//visibility:public"],
)