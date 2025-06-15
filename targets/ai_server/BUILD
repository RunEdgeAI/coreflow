cc_library(
    name = "ai-server",
    srcs = glob([
        "*.cpp",
        "*.h",
    ]),
    includes = [
        ".",
        "//framework/include",
        "//kernels/ai-server",
    ],
    deps = [
        "//:corevx",
        "//kernels/ai_server:llm_kernels"
    ],
    visibility = ["//visibility:public"]
)

cc_shared_library(
    name = "openvx-ai_server",
    deps = [
        ":ai-server",
    ],
    visibility = ["//visibility:public"]
)

cc_import(
    name = "imported_openvx_ai_server",
    shared_library = ":openvx-ai_server",
    visibility = ["//visibility:public"]
)
