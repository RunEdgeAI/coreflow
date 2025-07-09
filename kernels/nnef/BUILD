
# Build library for the nnef cpp parser
cc_library(
    name = "nnef",
    srcs = glob([
        "NNEF-Tools/nnef-pyproject/nnef/cpp/src/*.cpp"
    ]),
    hdrs = glob([
        "NNEF-Tools/nnef-pyproject/nnef/cpp/include/**/*.h",
    ]),
    includes = [
        "NNEF-Tools/nnef-pyproject/nnef/cpp/include/",
    ],
    visibility = ["//visibility:public"],
)