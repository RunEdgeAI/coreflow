
cc_library(
    name = "tiling",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h"
    ]),
    copts = [
        "-mfpu=neon",
        "-mfloat-abi=hard",
        "-march=armv8-a",
    ],
    includes = [
        ".",
        "//framework/include",
        "//kernels/tiling",
    ],
    deps = [
        "//:corevx",
        "//kernels/tiling:tiling_kernels"
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-tiling",
    deps = [
        ":tiling"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_tiling",
    shared_library = ":openvx-tiling",  # Correctly import the shared library
    visibility = ["//visibility:public"]
)