workspace(name = 'coreVX')

# Load Bazel's rules for multiple languages
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Example: C++ toolchain
http_archive(
    name = "rules_cc",
    urls = [
        "https://github.com/bazelbuild/rules_cc/archive/refs/tags/0.0.2.tar.gz",
    ],
    strip_prefix = "rules_cc-0.0.2",
)

load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
rules_cc_dependencies()

# Example: Python toolchain
http_archive(
    name = "rules_python",
    urls = ["https://github.com/bazelbuild/rules_python/archive/refs/tags/0.1.0.tar.gz"],
    strip_prefix = "rules_python-0.1.0",
)

load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

# Example: Java toolchain
# http_archive(
#     name = "rules_java",
#     urls = ["https://github.com/bazelbuild/rules_java/releases/download/5.1.0/rules_java-5.1.0.tar.gz"],
#     strip_prefix = "rules_java-5.1.0",
# )

# load("@rules_java//java:repositories.bzl", "rules_java_dependencies")
# rules_java_dependencies()

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
