
cc_library(
    name = "opencl",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/opencl",
        "//kernels/utils",
        "@opencl_headers//"
    ],
    deps = [
        "//:corevx",
        "//targets/debug:debug",
        "//targets/extras:extras",
    ],
    copts = [
        "-DCL_VERSION_1_2"
    ],
    linkopts = [
        "-lOpenCL"
    ],

    visibility = ["//visibility:public"]
)

# cc_shared_library(
#     name = "openvx-opencl",
#     deps = [
#         ":opencl"
#     ],
#     visibility = ["//visibility:public"]
# )

# cc_import(
#     name = "imported_openvx_opencl",
#     shared_library = ":openvx-opencl",  # Correctly import the shared library
#     visibility = ["//visibility:public"]
# )
