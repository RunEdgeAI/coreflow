# Build shared library for the main project
cc_shared_library(
    name = "openvx",
    deps = [":corevx"],
)

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
    deps = [
        "//kernels/NNEF-Tools/parser/cpp:nnef"
    ],
    visibility = ["//visibility:public"],
)