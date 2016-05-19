#pragma once

#include <string>

namespace Tangram {

// This class is based on the URL concept specified by IETF RFC 1808
// (https://tools.ietf.org/html/rfc1808). In particular this class is
// intended to handle URLs using the 'http' and 'file' schemes, with
// special case handling of some data URIs.
//
// URLs are decomposed as:
//
// foo://user:pword@host.com:80/over/there;type=a?name=ferret#nose
// \_/   \____________________/\_________/ \____/ \_________/ \__/
//  |              |               |         |         |       |
// scheme      netLocation        path   parameters  query  fragment
//
// Data URIs are decomposed as:
//
// data:image/png;base64,iVBORw0KGgoAAAANSUhE... (abbreviated)
// \__/ \_______/ \____/ \__________________ _ _
//  |       |       |         |
// scheme mediaType isBase64 data
//
// Ideas were borrowed from:
// https://github.com/cpp-netlib/uri/blob/master/include/network/uri/uri.hpp
// https://github.com/opensource-apple/CF/blob/master/CFURL.h
//
// Author: Matt Blair, 2016

class Url {

public:

    // Create an absolute or relative URL from a string.
    Url(const std::string& source);

    // Create an absolute URL from a string and a base URL.
    Url(const std::string& source, const Url& base);

    // Create an empty URL.
    Url();

    // Create a URL by copy.
    Url(const Url& other);

    // Create a URL by move.
    Url(Url&& other);

    // Query the state of this URL.
    bool isEmpty() const;
    bool isInvalid() const;
    bool isAbsolute() const;
    bool isStandardized() const;
    bool hasHttpScheme() const;
    bool hasFileScheme() const;
    bool hasDataScheme() const;
    bool hasBase64Data() const;

    // Query the presence of URL components.
    bool hasScheme() const;
    bool hasNetLocation() const;
    bool hasPath() const;
    bool hasParameters() const;
    bool hasQuery() const;
    bool hasFragment() const;

    // Query the presence of data URI components.
    bool hasMediaType() const;
    bool hasData() const;

    // Get copies of URL components.
    std::string scheme() const;
    std::string netLocation() const;
    std::string path() const;
    std::string parameters() const;
    std::string query() const;
    std::string fragment() const;

    // Get copies of data URI components.
    std::string mediaType() const;
    std::string data() const;

    // Get the entire URL as a string.
    const std::string& string() const;

    // Get an equivalent URL with dot segments removed from the path. If this is
    // a data URI then the same URI is returned.
    Url standardized() const;

    // Get a relative URL that resolves to the current URL against the given base.
    // e.g. "b/c.txt" == ("example.com/a/b/c.txt").makeRelative("example.com/a/")
    Url madeRelative(const Url& base) const;

    // Get an absolute URL by applying the current URL relative to the given base.
    // e.g. "example.com/a/b/c.txt" == ("b/c.txt").resolve("example.com/a/")
    Url resolved(const Url& base) const;

    // Compare this URL and another using their string representations.
    bool operator==(const Url& other) const;

    // Remove any '.' or '..' segments from a string range containing a heirarchical
    // path and return the pointer past the end of the new path.
    static char* removeDotSegmentsFromRange(char* begin, char* end);

    // Remove any '.' or '..' segments from a string containing a heirarchical path
    // and return a modified copy of the string.
    static std::string removeDotSegmentsFromString(std::string path);

private:

    bool initializeFromString(const std::string& source);

    // m_string contains the actual text of the URL.
    std::string m_string;

    // m_parts describes URL components by their location within the source string.
    struct PartRanges {
        struct Range {
            size_t start = 0, count = 0;
        } scheme, location, path, parameters, query, fragment, media, data;
    } m_parts;

    // m_flags contains Boolean information about the URL state.
    int m_flags = 0;

    // m_flags uses these bitmasks.
    enum {
        IS_INVALID =      1 << 0,
        IS_ABSOLUTE =     1 << 1,
        IS_STANDARDIZED = 1 << 2,
        HAS_HTTP_SCHEME = 1 << 3,
        HAS_FILE_SCHEME = 1 << 4,
        HAS_DATA_SCHEME = 1 << 5,
        HAS_BASE64_DATA = 1 << 6,
    };

    Url(std::string string, PartRanges parts, int flags);

}; // class Url

} // namespace Tangram
