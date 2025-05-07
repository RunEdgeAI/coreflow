
cc_library(
    name = "torch",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/torch",
    ],
    deps = [
        "//:corevx",
        "//kernels/executorch:torch_kernels"
    ],
    alwayslink = True,
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-torch",
    deps = [
        ":torch",
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_torch",
    shared_library = ":openvx-torch",
    visibility = ["//visibility:public"]
)
