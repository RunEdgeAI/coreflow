
cc_library(
    name = "ai-server",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
        # "//kernels/ai-server",
    ],
    deps = [
        "//:corevx",
        # "//kernels/ai-server:ai-server-kernels",
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-ai-server",
    deps = [
        ":ai-server",
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_ai_server",
    shared_library = ":openvx-ai-server",
    visibility = ["//visibility:public"]
)
