
cc_library(
    name = "venum",
    srcs = glob([
        "*.cpp",
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/venum",
    ],
    copts = [
        "-mfpu=neon",
        "-mfloat-abi=hard",
        "-march=armv8-a",
    ],
    deps = [
        "//:corevx",
        "//kernels/utils",
        "//kernels/venum:venum_kernels"
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-venum",
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