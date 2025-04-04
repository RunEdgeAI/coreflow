cc_library(
    name = "llm_kernels",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "*.hpp",
    ]),
    includes = [
        ".",
        "//framework/include"
    ],
    deps = [
        "//:corevx",
        "@curl//:curl",
        "@nlohmann_json//:json"
    ],
    visibility = ["//visibility:public"]
)