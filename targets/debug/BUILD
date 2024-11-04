
cc_library(
    name = "debug",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/debug",
        "//kernels/utils"
    ],
    deps = [
        "//:corevx",
        "//kernels/debug:debug_kernels"
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-debug",
    deps = [
        ":debug"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_debug",
    shared_library = ":openvx-debug",  # Correctly import the shared library
    visibility = ["//visibility:public"]
)