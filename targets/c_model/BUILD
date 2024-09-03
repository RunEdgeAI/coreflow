
cc_library(
    name = "c_model",
    srcs = glob([
        "*.cpp",
        "*.h",
    ],
    exclude = [
        "vx_canny.cpp",
        "vx_convolution.cpp",
        "vx_controlflow.cpp",
        "vx_copy.cpp",
        "vx_histogram.cpp",
        "vx_nonlinear*.cpp",
        "vx_o*.cpp",
        "vx_py*.cpp",
        "vx_remap.cpp",
        "vx_s*.cpp",
        "vx_th*.cpp",
        "vx_warp.cpp",
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/c_model",
        "//kernels/utils"
    ],
    deps = [
        "//:corevx",
        "//kernels/c_model:c_model_kernels",
        "//targets/extras:extras",
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-c_model",
    shared_lib_name = "libopenvx-c_model.dylib",
    deps = [
        ":c_model"
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_c_model",
    shared_library = ":openvx-c_model",  # Correctly import the shared library
    visibility = ["//visibility:public"]
)

