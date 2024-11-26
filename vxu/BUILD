
cc_shared_library(
    name = "vxu",
    deps = [
        ":corevx-vxu"
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "corevx-vxu",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
    ],
    deps = [
        "//:corevx",
    ],
    visibility = ["//visibility:public"]
)
