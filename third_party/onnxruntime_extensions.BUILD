cc_library(
    name = "headers",
    hdrs = glob(["includes/**"]),
    strip_include_prefix = "includes",
    deps = [
        "@github_nlohmann_json//:json",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "operators",
    hdrs = [
        "operators/base64.h",
        "operators/ustring.h",
        "operators/tokenizer/bert_tokenizer.hpp",
        "operators/tokenizer/basic_tokenizer.hpp"
    ] + glob([
        "operators/string_utils*",
    ]),
    srcs = [
        "operators/base64.cc",
        "operators/ustring.cc",
        "operators/tokenizer/bert_tokenizer.cc",
        "operators/tokenizer/basic_tokenizer.cc"
    ] + glob([
        "operators/string_utils*",
    ]),
    strip_include_prefix = "operators",
    visibility = ["//visibility:public"],
)