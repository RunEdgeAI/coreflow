

cc_import(
    name = "import_vx_pop",
    shared_library = ":vx_pop",
)

cc_shared_library(
    name = "vx_pop",
    deps = [":vx_pop_a"]
)

cc_library(
    name = "vx_pop_a",
    srcs = [
        "source/vx_pop_bubble.cpp",
        "source/vx_pop_donut.cpp",
        "source/internal_dataTranslator.cpp",
        "source/internal_publishKernels.cpp",
	    "source/internal_vxNodes.cpp"
    ],
    deps = [
        "//:corevx",
        "@opencv",
        "//targets/ai_server:imported_openvx_ai_server",
        "//targets/c_model:imported_openvx_c_model",
        "//targets/debug:imported_openvx_debug",
        "//targets/extras:imported_openvx_extras",
        "//targets/liteRT:imported_openvx_liteRT",
        "//targets/opencl:imported_openvx_opencl",
        "//targets/onnxRT:imported_openvx_onnxRT",
        "//targets/executorch:imported_openvx_torch",
        "//third_party:opencv-prebuilt",
    ],
    hdrs = glob([
        "include/*.h"
    ]),
    includes = ["include"],
)

cc_binary(
    name = "bubble-pop",
    srcs = [
        "source/bubble_pop.cpp",
        "include/vx_ext_pop.h",
        "include/vx_pop.h",
    ],
    includes = ["include"],
    data = [
        "image/bubble.png",
    ],
    deps = [
        ":vx_pop_a",
        ":import_vx_pop"
    ],
    visibility = ["//visibility:public"],
)