
cc_library(
    name = "onnxRT",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/onnxRT",
        "//kernels/utils"
    ],
    deps = [
        "//:corevx",
        "//kernels/onnxRT:ort_kernels",
    ],
    linkstatic = True,
    alwayslink = True,
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-onnxRT",
    deps = [
        ":onnxRT",
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_onnxRT",
    shared_library = ":openvx-onnxRT",
    visibility = ["//visibility:public"]
)
