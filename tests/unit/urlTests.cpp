#include "catch.hpp"

#include "util/url.h"

using namespace Tangram;

TEST_CASE("Parse components of a correctly formatted URL", "[Url]") {

    // Tests conformance to https://tools.ietf.org/html/rfc1808#section-2.1

    Url url("https://vector.mapzen.com/osm/all/0/0/0.mvt;param=val?api_key=mapsRcool#yolo");

    CHECK(!url.isEmpty());
    CHECK(url.isAbsolute());
    CHECK(!url.hasDataScheme());
    CHECK(!url.hasBase64Data());
    CHECK(!url.hasFileScheme());
    CHECK(url.hasHttpScheme());
    CHECK(url.hasScheme());
    CHECK(url.scheme() == "https");
    CHECK(url.hasNetLocation());
    CHECK(url.netLocation() == "vector.mapzen.com");
    CHECK(url.hasPath());
    CHECK(url.path() == "/osm/all/0/0/0.mvt");
    CHECK(url.hasParameters());
    CHECK(url.parameters() == "param=val");
    CHECK(url.hasQuery());
    CHECK(url.query() == "api_key=mapsRcool");
    CHECK(url.hasFragment());
    CHECK(url.fragment() == "yolo");
    CHECK(!url.hasMediaType());
    CHECK(!url.hasData());

}

TEST_CASE("Parse components of a correctly formatted data URI", "[Url]") {

    // Tests conformance to https://tools.ietf.org/html/rfc2397#section-3

    Url url("data:text/html;charset=utf-8;base64,YmFzZTY0");

    CHECK(!url.isEmpty());
    CHECK(url.isAbsolute());
    CHECK(url.hasDataScheme());
    CHECK(url.hasBase64Data());
    CHECK(!url.hasFileScheme());
    CHECK(!url.hasHttpScheme());
    CHECK(url.hasScheme());
    CHECK(url.scheme() == "data");
    CHECK(!url.hasNetLocation());
    CHECK(!url.hasParameters());
    CHECK(!url.hasQuery());
    CHECK(!url.hasFragment());
    CHECK(url.hasMediaType());
    CHECK(url.mediaType() == "text/html;charset=utf-8");
    CHECK(url.hasData());
    CHECK(url.data() == "YmFzZTY0");

}

TEST_CASE("Parse an empty URL", "[URL]") {

    Url url("");

    CHECK(url.isEmpty());
    CHECK(!url.isAbsolute());
    CHECK(!url.hasDataScheme());
    CHECK(!url.hasBase64Data());
    CHECK(!url.hasScheme());
    CHECK(!url.hasNetLocation());
    CHECK(!url.hasPath());
    CHECK(!url.hasParameters());
    CHECK(!url.hasQuery());
    CHECK(!url.hasFragment());
    CHECK(!url.hasMediaType());
    CHECK(!url.hasData());

}

TEST_CASE("Remove dot segments from a path", "[Url]") {

    // Tests conformance to https://tools.ietf.org/html/rfc3986#section-5.2.4

    CHECK(Url::removeDotSegmentsFromString("") == "");
    CHECK(Url::removeDotSegmentsFromString("a/b/c") == "a/b/c");
    CHECK(Url::removeDotSegmentsFromString("a/b=?.;5/c") == "a/b=?.;5/c");
    CHECK(Url::removeDotSegmentsFromString("/a/b/c/./../../g") == "/a/g");
    CHECK(Url::removeDotSegmentsFromString("../a/b") == "a/b");
    CHECK(Url::removeDotSegmentsFromString("./") == "");
    CHECK(Url::removeDotSegmentsFromString("a/b/../../..") == "");
    CHECK(Url::removeDotSegmentsFromString("a/b/../c/../d/./e/..") == "a/d");
    CHECK(Url::removeDotSegmentsFromString("a//b//c") == "a//b//c");
    CHECK(Url::removeDotSegmentsFromString("a/b../..c/..d") == "a/b../..c/..d");

}

TEST_CASE("Produce a 'standardized' URL", "[URL]") {

    CHECK(Url("http://example.com/path/oops/not/here/../../../file.txt").standardized().string() == "http://example.com/path/file.txt");
    CHECK(Url("http://example.com/../../no/going/back/file.txt").standardized().string() == "http://example.com/no/going/back/file.txt");
    CHECK(Url("data:text/html;charset=utf-8,LoremIpsum").standardized().string() == "data:text/html;charset=utf-8,LoremIpsum");

}

TEST_CASE("Maintain URL components when 'standardized'", "[URL]") {

    Url url(Url("http://mapzen.com/nothing/to/see/here/../../../../index.html;p?q#f").standardized());

    CHECK(!url.isEmpty());
    CHECK(url.isAbsolute());
    CHECK(!url.hasDataScheme());
    CHECK(!url.hasBase64Data());
    CHECK(!url.hasFileScheme());
    CHECK(url.hasHttpScheme());
    CHECK(url.hasScheme());
    CHECK(url.scheme() == "http");
    CHECK(url.hasNetLocation());
    CHECK(url.netLocation() == "mapzen.com");
    CHECK(url.hasPath());
    CHECK(url.path() == "/index.html");
    CHECK(url.hasParameters());
    CHECK(url.parameters() == "p");
    CHECK(url.hasQuery());
    CHECK(url.query() == "q");
    CHECK(url.hasFragment());
    CHECK(url.fragment() == "f");
    CHECK(!url.hasMediaType());
    CHECK(!url.hasData());

}
