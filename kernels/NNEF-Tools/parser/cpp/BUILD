# Build library for the nnef cpp parser
cc_library(
    name = "nnef",
    srcs = glob([
       # "*.cpp",
        "src/*.cpp"
    ]),
    hdrs = glob([
        "include/**",
        "include/*.h",
    ]),
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)