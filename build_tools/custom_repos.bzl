""""
custom_repos.bzl
"""
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def custom_git_repository(name, remote, branch=None, commit=None):
    git_repository(
        name = name,
        remote = remote,
        branch = branch,
        commit = commit,
    )

def custom_http_archive(name, url, strip_prefix, sha256, build_file_content=None):
    http_archive(
        name = name,
        url = url,
        strip_prefix = strip_prefix,
        sha256 = sha256,
        build_file_content = build_file_content,
    )
