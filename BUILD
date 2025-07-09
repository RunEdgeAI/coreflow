# Build shared library for the main project
cc_shared_library(
    name = "openvx",
    deps = [":corevx"],
    visibility = ["//visibility:public"],
)

# Build library for the main project
cc_library(
    name = "corevx",
    srcs = glob([
        "framework/src/*.cpp"
    ]),
    hdrs = glob([
        "include/**",
        "framework/include/*.h*",
    ]),
    includes = [
        "include",
        "framework/include"
    ],
    copts = select({
        "@platforms//os:linux": [
            "-I/usr/include/libxml2",
        ],
        "//conditions:default": [
        ],
    }),
    linkopts = select({
        "@platforms//os:osx": [
            "-framework OpenCL",
        ],
        "//conditions:default": [
            "-lOpenCL",
        ],
    }) + [
        "-lxml2"
    ],
    deps = [
        "//kernels/nnef:nnef"
    ],
    visibility = ["//visibility:public"],
)