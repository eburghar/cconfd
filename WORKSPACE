load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

http_archive(
    name = "jsonnet",
    build_file_content = all_content,
    sha256 = "e87a8b7074f2ddfafc11aef9118695054cf6d2f7f12d2fb6d862d5dcde8de2d4",
    strip_prefix = "jsonnet-0.16.0",
    urls = ["https://github.com/google/jsonnet/archive/v0.16.0.zip"]
)

http_archive(
    name = "json",
    sha256 = "6bea5877b1541d353bd77bdfbdb2696333ae5ed8f9e8cc22df657192218cad91",
    strip_prefix = "include",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip"],
    build_file = "@//:deps/json.BUILD"
)

http_archive(
    name = "args",
    sha256 = "f43c625a90d186b73dd23f5dd6b49142bee16225f46266d624f65f4715762a1d",
    strip_prefix = "args-6.2.3",
    urls = ["https://github.com/Taywee/args/archive/6.2.3.zip"],
    build_file = "@//:deps/args.BUILD"
)

http_archive(
    name = "googletest",
    sha256 = "94c634d499558a76fa649edb13721dce6e98fb1e7018dfaeba3cd7a083945e91",
    strip_prefix = "googletest-release-1.10.0",
    urls = ["https://github.com/google/googletest/archive/release-1.10.0.zip"],
    build_file = "@//:deps/googletest.BUILD",
)

http_archive(
    name = "spdlog",
    sha256 = "eed0095a1d52d08a0834feda146d4f9148fa4125620cd04d8ea57e0238fa39cd",
    strip_prefix = "spdlog-1.8.1",
    urls = ["https://github.com/gabime/spdlog/archive/v1.8.1.zip"],
    build_file = "@//:deps/spdlog.BUILD",
)

http_archive(
    name = "backward",
    sha256 = "d04f1900165d8909011d8917b9c308b3066612f45f689769810305d849ad7425",
    strip_prefix = "backward-cpp-1.4",
    urls = ["https://github.com/bombela/backward-cpp/archive/v1.4.zip"],
    build_file = "@//:deps/backward.BUILD",
    patches = ["@//:deps/backward.patch"],
)

http_archive(
    name="fruit",
    sha256 = "8c700fdebff7ba479e298f6b0a4691aa2325c6168c9cb98a4baa59e33dba5d65",
    strip_prefix = "fruit-3.4.0",
    urls = ["https://github.com/google/fruit/archive/v3.4.0.zip"]
)

http_archive(
    name="date",
    sha256 = "9278d7568afdf6fb880f277d5e344f92c0531459f15643bc945849c2f209dab2",
    strip_prefix = "date-2.4.1",
    urls = ["https://github.com/howardhinnant/date/archive/v2.4.1.zip"],
    build_file = "@//:deps/date.BUILD",
)
