#include <mbgl/test/util.hpp>

#include <mbgl/util/url.hpp>

#include <memory>

using namespace mbgl::util;

TEST(URL, Scheme) {
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("http://example.com/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("http://127.0.0.1:8080/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("http://[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("http://user:password@example.com.:80/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("htt").scheme);
    EXPECT_EQ(URL::Segment({ 0, 6 }), URL("mapbox://").scheme);
    EXPECT_EQ(URL::Segment({ 0, 6 }), URL("mapbox:/#").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("://").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("http?query://baz").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL(":::").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("127.0.0.1:8080/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").scheme);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("data:,Hello%2C%20World!").scheme);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ%3D%3D").scheme);
}

TEST(URL, Query) {
    EXPECT_EQ(URL::Segment({ 23, 10 }), URL("http://example.com/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 26, 10 }), URL("http://127.0.0.1:8080/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 47, 10 }), URL("http://[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 41, 10 }), URL("http://user:password@example.com.:80/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 23, 10 }), URL("http://example.com/test?query=foo#page-2").query);
    EXPECT_EQ(URL::Segment({ 23, 0 }), URL("http://example.com/test#query=foo?page-2").query);
    EXPECT_EQ(URL::Segment({ 0, 10 }), URL("?query=foo").query);
    EXPECT_EQ(URL::Segment({ 0, 10 }), URL("?query=foo#bar").query);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("#?query=foo#bar").query);
    EXPECT_EQ(URL::Segment({ 4, 12 }), URL("http?query://baz").query);
    EXPECT_EQ(URL::Segment({ 9, 4 }), URL("mapbox://?bar").query);
    EXPECT_EQ(URL::Segment({ 12, 0 }), URL("mapbox://bar").query);
    EXPECT_EQ(URL::Segment({ 0, 0 }), URL("").query);
    EXPECT_EQ(URL::Segment({ 3, 0 }), URL(":::").query);
    EXPECT_EQ(URL::Segment({ 19, 10 }), URL("127.0.0.1:8080/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 40, 10 }), URL("[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").query);
    EXPECT_EQ(URL::Segment({ 23, 0 }), URL("data:,Hello%2C%20World!").query);
    EXPECT_EQ(URL::Segment({ 47, 0 }), URL("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ%3D%3D").query);
}

TEST(URL, Domain) {
    EXPECT_EQ(URL::Segment({ 7, 11 }), URL("http://example.com/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 7, 14 }), URL("http://127.0.0.1:8080/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 7, 35 }), URL("http://[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 7, 29 }), URL("http://user:password@example.com.:80/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 5, 11 }), URL("http:example.com/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 0, 3 }), URL("htt").domain);
    EXPECT_EQ(URL::Segment({ 0, 4 }), URL("http?query://baz").domain);
    EXPECT_EQ(URL::Segment({ 0, 6 }), URL("domain/foo?bar").domain);
    EXPECT_EQ(URL::Segment({ 7, 0 }), URL("http://").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain#bar").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain/").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain/foo").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain/foo?").domain);
    EXPECT_EQ(URL::Segment({ 0, 1 }), URL("h##p://domain/foo?bar").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain/foo?bar").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain/?").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain?").domain);
    EXPECT_EQ(URL::Segment({ 7, 6 }), URL("http://domain?foo").domain);
    EXPECT_EQ(URL::Segment({ 3, 0 }), URL(":::").domain);
    EXPECT_EQ(URL::Segment({ 0, 14 }), URL("127.0.0.1:8080/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 0, 35 }), URL("[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").domain);
    EXPECT_EQ(URL::Segment({ 5, 0 }), URL("data:,Hello%2C%20World!").domain);
    EXPECT_EQ(URL::Segment({ 5, 17 }), URL("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ%3D%3D").domain);
}

TEST(URL, Path) {
    EXPECT_EQ(URL::Segment({ 18, 5 }), URL("http://example.com/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 21, 5 }), URL("http://127.0.0.1:8080/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 42, 5 }), URL("http://[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 36, 5 }), URL("http://user:password@example.com.:80/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 18, 5 }), URL("http://example.com/test?query=foo#bar").path);
    EXPECT_EQ(URL::Segment({ 18, 5 }), URL("http://example.com/test#bar").path);
    EXPECT_EQ(URL::Segment({ 18, 0 }), URL("http://example.com?query=foo").path);
    EXPECT_EQ(URL::Segment({ 18, 0 }), URL("http://example.com#?query=foo").path);
    EXPECT_EQ(URL::Segment({ 18, 1 }), URL("http://example.com/?query=foo").path);
    EXPECT_EQ(URL::Segment({ 3, 0 }), URL(":::").path);
    EXPECT_EQ(URL::Segment({ 14, 5 }), URL("127.0.0.1:8080/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 35, 5 }), URL("[2a01:4f8:c17:3680::386a:6f3d]:8080/test?query=foo").path);
    EXPECT_EQ(URL::Segment({ 13, 0 }), URL("http://domain").path);
    EXPECT_EQ(URL::Segment({ 6, 4 }), URL("domain/foo?bar").path);
    EXPECT_EQ(URL::Segment({ 6, 17 }), URL("data:,Hello%2C%20World!").path);
    EXPECT_EQ(URL::Segment({ 23, 24 }), URL("data:text/plain;base64,SGVsbG8sIFdvcmxkIQ%3D%3D").path);
}

auto URLPath = [](const char* str) { const URL url(str); return Path(str, url.path.first, url.path.second); };

TEST(Path, Directory) {
    EXPECT_EQ(Path::Segment({ 0, 8 }), Path("foo/bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 8 }), Path("foo.bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 8 }), Path("foo.bar/baz").directory);
    EXPECT_EQ(Path::Segment({ 0, 8 }), Path("foo/bar/.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 4 }), Path("foo/bar@2x.png").directory);
    EXPECT_EQ(Path::Segment({ 0, 4 }), Path("foo/b").directory);
    EXPECT_EQ(Path::Segment({ 0, 4 }), Path("foo/").directory);
    EXPECT_EQ(Path::Segment({ 0, 0 }), Path("foo").directory);
    EXPECT_EQ(Path::Segment({ 0, 0 }), Path("foo.png").directory);
    EXPECT_EQ(Path::Segment({ 0, 0 }), Path("").directory);

    EXPECT_EQ(Path::Segment({ 0, 9 }), Path("/foo/bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 9 }), Path("/foo.bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 9 }), Path("/foo.bar/baz").directory);
    EXPECT_EQ(Path::Segment({ 0, 9 }), Path("/foo/bar/.ext").directory);
    EXPECT_EQ(Path::Segment({ 0, 5 }), Path("/foo/bar@2x.png").directory);
    EXPECT_EQ(Path::Segment({ 0, 5 }), Path("/foo/b").directory);
    EXPECT_EQ(Path::Segment({ 0, 5 }), Path("/foo/").directory);
    EXPECT_EQ(Path::Segment({ 0, 1 }), Path("/foo").directory);
    EXPECT_EQ(Path::Segment({ 0, 1 }), Path("/foo.png").directory);
    EXPECT_EQ(Path::Segment({ 0, 1 }), Path("/").directory);
}

TEST(Path, URLDirectory) {
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo/bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo/bar/baz.ext?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo.bar/baz.ext").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo.bar/baz.ext?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo.bar/baz").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo.bar/baz?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo/bar/.ext").directory);
    EXPECT_EQ(Path::Segment({ 18, 9 }), URLPath("http://example.com/foo/bar/.ext?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/bar@2x.png").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/bar@2x.png?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/b").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/b?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/").directory);
    EXPECT_EQ(Path::Segment({ 18, 5 }), URLPath("http://example.com/foo/?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/foo").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/foo?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/foo.png").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/foo.png?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/").directory);
    EXPECT_EQ(Path::Segment({ 18, 1 }), URLPath("http://example.com/?query=foo.bar").directory);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com").directory);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com?query=foo.bar").directory);
}

TEST(Path, Extension) {
    EXPECT_EQ(Path::Segment({ 11, 4 }), Path("foo/bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 11, 4 }), Path("foo.bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 18, 4 }), Path("foo.bar/baz.vector.pbf").extension);
    EXPECT_EQ(Path::Segment({ 11, 0 }), Path("foo.bar/baz").extension);
    EXPECT_EQ(Path::Segment({ 8, 4 }), Path("foo/bar/.ext").extension);
    EXPECT_EQ(Path::Segment({ 7, 7 }), Path("foo/bar@2x.png").extension);
    EXPECT_EQ(Path::Segment({ 5, 0 }), Path("foo/b").extension);
    EXPECT_EQ(Path::Segment({ 4, 0 }), Path("foo/").extension);
    EXPECT_EQ(Path::Segment({ 3, 0 }), Path("foo").extension);
    EXPECT_EQ(Path::Segment({ 3, 4 }), Path("foo.png").extension);
    EXPECT_EQ(Path::Segment({ 0, 0 }), Path("").extension);

    EXPECT_EQ(Path::Segment({ 12, 4 }), Path("/foo/bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 12, 4 }), Path("/foo.bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 19, 4 }), Path("/foo.bar/baz.vector.pbf").extension);
    EXPECT_EQ(Path::Segment({ 12, 0 }), Path("/foo.bar/baz").extension);
    EXPECT_EQ(Path::Segment({ 9, 4 }), Path("/foo/bar/.ext").extension);
    EXPECT_EQ(Path::Segment({ 8, 7 }), Path("/foo/bar@2x.png").extension);
    EXPECT_EQ(Path::Segment({ 6, 0 }), Path("/foo/b").extension);
    EXPECT_EQ(Path::Segment({ 5, 0 }), Path("/foo/").extension);
    EXPECT_EQ(Path::Segment({ 4, 0 }), Path("/foo").extension);
    EXPECT_EQ(Path::Segment({ 4, 4 }), Path("/foo.png").extension);
    EXPECT_EQ(Path::Segment({ 1, 0 }), Path("/").extension);
}

TEST(Path, URLExtension) {
    EXPECT_EQ(Path::Segment({ 30, 4 }), URLPath("http://example.com/foo/bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 30, 4 }), URLPath("http://example.com/foo/bar/baz.ext?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 30, 4 }), URLPath("http://example.com/foo.bar/baz.ext").extension);
    EXPECT_EQ(Path::Segment({ 30, 4 }), URLPath("http://example.com/foo.bar/baz.ext?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 30, 0 }), URLPath("http://example.com/foo.bar/baz").extension);
    EXPECT_EQ(Path::Segment({ 30, 0 }), URLPath("http://example.com/foo.bar/baz?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 27, 4 }), URLPath("http://example.com/foo/bar/.ext").extension);
    EXPECT_EQ(Path::Segment({ 27, 4 }), URLPath("http://example.com/foo/bar/.ext?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 26, 7 }), URLPath("http://example.com/foo/bar@2x.png").extension);
    EXPECT_EQ(Path::Segment({ 26, 7 }), URLPath("http://example.com/foo/bar@2x.png?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 24, 0 }), URLPath("http://example.com/foo/b").extension);
    EXPECT_EQ(Path::Segment({ 24, 0 }), URLPath("http://example.com/foo/b?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 23, 0 }), URLPath("http://example.com/foo/").extension);
    EXPECT_EQ(Path::Segment({ 23, 0 }), URLPath("http://example.com/foo/?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 22, 0 }), URLPath("http://example.com/foo").extension);
    EXPECT_EQ(Path::Segment({ 22, 0 }), URLPath("http://example.com/foo?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 22, 4 }), URLPath("http://example.com/foo.png").extension);
    EXPECT_EQ(Path::Segment({ 22, 4 }), URLPath("http://example.com/foo.png?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 19, 0 }), URLPath("http://example.com/").extension);
    EXPECT_EQ(Path::Segment({ 19, 0 }), URLPath("http://example.com/?query=foo.bar").extension);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com").extension);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com?query=foo.bar").extension);
}

TEST(Path, Filename) {
    EXPECT_EQ(Path::Segment({ 8, 3 }), Path("foo/bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 8, 3 }), Path("foo.bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 8, 3 }), Path("foo.bar/baz").filename);
    EXPECT_EQ(Path::Segment({ 8, 0 }), Path("foo/bar/.ext").filename);
    EXPECT_EQ(Path::Segment({ 4, 3 }), Path("foo/bar@2x.png").filename);
    EXPECT_EQ(Path::Segment({ 4, 1 }), Path("foo/b").filename);
    EXPECT_EQ(Path::Segment({ 4, 0 }), Path("foo/").filename);
    EXPECT_EQ(Path::Segment({ 0, 3 }), Path("foo").filename);
    EXPECT_EQ(Path::Segment({ 0, 3 }), Path("foo.png").filename);
    EXPECT_EQ(Path::Segment({ 0, 0 }), Path("").filename);

    EXPECT_EQ(Path::Segment({ 9, 3 }), Path("/foo/bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 9, 3 }), Path("/foo.bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 9, 3 }), Path("/foo.bar/baz").filename);
    EXPECT_EQ(Path::Segment({ 9, 0 }), Path("/foo/bar/.ext").filename);
    EXPECT_EQ(Path::Segment({ 5, 3 }), Path("/foo/bar@2x.png").filename);
    EXPECT_EQ(Path::Segment({ 5, 1 }), Path("/foo/b").filename);
    EXPECT_EQ(Path::Segment({ 5, 0 }), Path("/foo/").filename);
    EXPECT_EQ(Path::Segment({ 1, 3 }), Path("/foo").filename);
    EXPECT_EQ(Path::Segment({ 1, 3 }), Path("/foo.png").filename);
    EXPECT_EQ(Path::Segment({ 1, 0 }), Path("/").filename);
}

TEST(Path, URLFilename) {
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo/bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo/bar/baz.ext?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo.bar/baz.ext").filename);
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo.bar/baz.ext?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo.bar/baz").filename);
    EXPECT_EQ(Path::Segment({ 27, 3 }), URLPath("http://example.com/foo.bar/baz?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 27, 0 }), URLPath("http://example.com/foo/bar/.ext").filename);
    EXPECT_EQ(Path::Segment({ 27, 0 }), URLPath("http://example.com/foo/bar/.ext?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 23, 3 }), URLPath("http://example.com/foo/bar@2x.png").filename);
    EXPECT_EQ(Path::Segment({ 23, 3 }), URLPath("http://example.com/foo/bar@2x.png?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 23, 1 }), URLPath("http://example.com/foo/b").filename);
    EXPECT_EQ(Path::Segment({ 23, 1 }), URLPath("http://example.com/foo/b?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 23, 0 }), URLPath("http://example.com/foo/").filename);
    EXPECT_EQ(Path::Segment({ 23, 0 }), URLPath("http://example.com/foo/?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 19, 3 }), URLPath("http://example.com/foo").filename);
    EXPECT_EQ(Path::Segment({ 19, 3 }), URLPath("http://example.com/foo?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 19, 3 }), URLPath("http://example.com/foo.png").filename);
    EXPECT_EQ(Path::Segment({ 19, 3 }), URLPath("http://example.com/foo.png?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 19, 0 }), URLPath("http://example.com/").filename);
    EXPECT_EQ(Path::Segment({ 19, 0 }), URLPath("http://example.com/?query=foo.bar").filename);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com").filename);
    EXPECT_EQ(Path::Segment({ 18, 0 }), URLPath("http://example.com?query=foo.bar").filename);
}
