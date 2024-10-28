



cc_library(
    name = "venum",
    srcs = glob([
        "*.cpp",
    ],
    exclude = [
    ]),
    hdrs = glob([
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/utils",
        "//kernels/venum",
    ],
    deps = [
        "//:corevx",
        "//kernels/venum:venum_kernels"
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-venum",
    shared_lib_name = "libopenvx-venum.dylib",
    deps = [
        ":venum"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_venum",
    shared_library = ":openvx-venum",  # Correctly import the shared library
    visibility = ["//visibility:public"]
)