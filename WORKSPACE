workspace(name = 'coreVX')

# Load Bazel's rules for multiple languages
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Example: C++ toolchain
http_archive(
    name = "rules_cc",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/refs/tags/0.0.2.tar.gz",],
    integrity = "sha256-DlgBsYNKUcEwgETpq662qvlF5KQnSGbqhPvMxQopI2g=",
    strip_prefix = "rules_cc-0.0.2",
)

load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
rules_cc_dependencies()

http_archive(
    name = "rules_foreign_cc",
    sha256 = "a2e6fb56e649c1ee79703e99aa0c9d13c6cc53c8d7a0cbb8797ab2888bbc99a3",
    strip_prefix = "rules_foreign_cc-0.12.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.12.0/rules_foreign_cc-0.12.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.12.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

load("@bazel_features//:deps.bzl", "bazel_features_deps")
bazel_features_deps()

# Example: Python toolchain
http_archive(
    name = "rules_python",
    urls = ["https://github.com/bazelbuild/rules_python/archive/refs/tags/0.1.0.tar.gz"],
    integrity = "sha256-SPfnFvQJi4Upatk/WhM7r3EpaME/vC/fOmE2FY/obqw=",
    strip_prefix = "rules_python-0.1.0",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Add additional languages or tools as needed

# Register the external dependencies in Bazel's dependency graph
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Example: Load a generic library from GitHub
git_repository(
    name = "some_generic_library",
    remote = "https://github.com/user/some_generic_library.git",
    branch = "main",
)

# Add other external dependencies or tools as needed

# # Fetch OpenCL Headers
# http_archive(
#     name = "opencl_headers",
#     url = "https://github.com/KhronosGroup/OpenCL-Headers/archive/refs/tags/v2023.04.17.tar.gz",
#     strip_prefix = "OpenCL-Headers-2023.04.17",
#     sha256 = "0ce992f4167f958f68a37918dec6325be18f848dee29a4521c633aae3304915d",
#     build_file_content = """
# cc_library(
#     name = "opencl_headers",
#     hdrs = glob(["CL/*.h"]),
#     includes = ["."],
#     visibility = ["//visibility:public"],
# )
# """,
# )

# Define PyTorch repository
git_repository(
    name = "pytorch",
    remote = "https://github.com/pytorch/pytorch.git",
    commit = "7c98e70d44abc7a1aead68b6ea6c8adc8c554db5" # v1.13.0
)
