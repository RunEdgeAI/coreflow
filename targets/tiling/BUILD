
cc_library(
    name = "tiling",
    srcs = glob([
        "*.cpp",
    ],
    exclude = [
    ]),
    hdrs = glob([
        "*.h"
    ]),
    copts = [
        "-mfpu=neon",  # Use NEON SIMD instructions
        # "-march=armv8-a",  # Specify the ARM architecture (modify based on your platform)
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