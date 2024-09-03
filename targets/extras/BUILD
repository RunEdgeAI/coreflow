

cc_library(
    name = "extras",
    srcs = glob([
        "*.cpp",
    ],
    exclude = [
        "vx_edge_trace.cpp"
    ]),
    hdrs = glob([
        "*.h"
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/extras",
    ],
    deps = [
        "//:corevx",
        "//kernels/extras:extras_kernels"
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-extras",
    shared_lib_name = "libopenvx-extras.dylib",
    deps = [
        ":extras"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_extras",
    shared_library = ":openvx-extras",  # Correctly import the shared library
    visibility = ["//visibility:public"]
)