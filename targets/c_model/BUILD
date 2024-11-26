
cc_library(
    name = "c_model",
    srcs = glob([
        "*.cpp",
        "*.h",
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
        "//targets/debug:debug",
        "//targets/extras:extras",
        "//vxu:corevx-vxu"
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-c_model",
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

