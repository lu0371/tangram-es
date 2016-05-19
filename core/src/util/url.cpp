#include "url.h"
#include <cstdlib>
#include <cassert>

namespace Tangram {

Url::Url(const std::string& source) {
    if (!initializeFromString(source)) {
        m_flags |= IS_INVALID;
    }
}

Url::Url(const std::string& source, const Url& base) :
    Url(Url(source).resolved(base)) {
}

Url::Url() {}

Url::Url(const Url& other) :
    m_string(other.m_string),
    m_parts(other.m_parts),
    m_flags(other.m_flags) {
}

Url::Url(Url&& other) :
    m_string(std::move(other.m_string)),
    m_parts(other.m_parts),
    m_flags(other.m_flags) {
}

Url::Url(std::string string, PartRanges parts, int flags) :
    m_string(string),
    m_parts(parts),
    m_flags(flags) {
}

bool Url::isEmpty() const {
    return m_string.empty();
}

bool Url::isInvalid() const {
    return (m_flags & IS_INVALID);
}

bool Url::isAbsolute() const {
    return (m_flags & IS_ABSOLUTE);
}

bool Url::isStandardized() const {
    return (m_flags & IS_STANDARDIZED);
}

bool Url::hasHttpScheme() const {
    return (m_flags & HAS_HTTP_SCHEME);
}

bool Url::hasFileScheme() const {
    return (m_flags & HAS_FILE_SCHEME);
}

bool Url::hasDataScheme() const {
    return (m_flags & HAS_DATA_SCHEME);
}

bool Url::hasBase64Data() const {
    return (m_flags & HAS_BASE64_DATA);
}

bool Url::hasScheme() const {
    return m_parts.scheme.count != 0;
}

bool Url::hasNetLocation() const {
    return m_parts.location.count != 0;
}

bool Url::hasPath() const {
    return m_parts.path.count != 0;
}

bool Url::hasParameters() const {
    return m_parts.parameters.count != 0;
}

bool Url::hasQuery() const {
    return m_parts.query.count != 0;
}

bool Url::hasFragment() const {
    return m_parts.fragment.count != 0;
}

bool Url::hasMediaType() const {
    return m_parts.media.count != 0;
}

bool Url::hasData() const {
    return m_parts.data.count != 0;
}

std::string Url::scheme() const {
    return std::string(m_string, m_parts.scheme.start, m_parts.scheme.count);
}

std::string Url::netLocation() const {
    return std::string(m_string, m_parts.location.start, m_parts.location.count);
}

std::string Url::path() const {
    return std::string(m_string, m_parts.path.start, m_parts.path.count);
}

std::string Url::parameters() const {
    return std::string(m_string, m_parts.parameters.start, m_parts.parameters.count);
}

std::string Url::query() const {
    return std::string(m_string, m_parts.query.start, m_parts.query.count);
}

std::string Url::fragment() const {
    return std::string(m_string, m_parts.fragment.start, m_parts.fragment.count);
}

std::string Url::mediaType() const {
    return std::string(m_string, m_parts.media.start, m_parts.media.count);
}

std::string Url::data() const {
    return std::string(m_string, m_parts.data.start, m_parts.data.count);
}

const std::string& Url::string() const {
    return m_string;
}

Url Url::standardized() const {

    // If this URL is already standardized, return a copy.
    if (isStandardized()) {
        return *this;
    }

    // If this is a data URI, return a copy.
    if (hasDataScheme()) {
        return *this;
    }

    // Start with the members of this URL.
    auto string = m_string;
    auto parts = m_parts;
    auto flags = m_flags;

    // Remove any dot segments from the path.
    char* pathStart = &string[parts.path.start];
    char* oldPathEnd = &string[parts.path.start + parts.path.count];
    char* newPathEnd = removeDotSegmentsFromRange(pathStart, oldPathEnd);

    if (newPathEnd != oldPathEnd) {

        // The new path should always be the same size or shorter.
        assert(oldPathEnd > newPathEnd);
        size_t offset = oldPathEnd - newPathEnd;

        // Adjust the size of the 'path' part.
        parts.path.count -= offset;

        // Remove any extra parts of the old path from the string.
        string.erase(parts.path.start + parts.path.count, offset);

        // Adjust the locations of the URL parts after 'path'.
        parts.parameters.start -= offset;
        parts.query.start -= offset;
        parts.fragment.start -= offset;
    }

    // Set the standardized flag.
    flags |= IS_STANDARDIZED;

    return Url(string, parts, flags);
}

Url Url::madeRelative(const Url& base) const {
    // TODO
    return *this;
}

Url Url::resolved(const Url& base) const {
    // TODO
    // https://tools.ietf.org/html/rfc1808#section-4
    // https://tools.ietf.org/html/rfc3986#section-5.2
    return *this;
}

bool Url::operator==(const Url& rhs) const {
    return m_string == rhs.m_string;
}

// Helper function for removeDotSegmentsFromRange
bool rangeStartsWith(const char* begin, const char* end, const char* prefix) {
    while (begin < end && (*prefix != '\0') && *begin == *prefix) { begin++; prefix++; }
    return (*prefix == '\0');
}

// Helper function for removeDotSegmentsFromRange
bool rangeEquals(const char* begin, const char* end, const char* prefix) {
    while (begin < end && (*prefix != '\0') && *begin == *prefix) { begin++; prefix++; }
    return (begin == end && *prefix == '\0');
}

// Helper function for removeDotSegmentsFromRange
bool rangeIsAll(const char* begin, const char* end, const char value) {
    while (begin < end && *begin == value) { begin++; }
    return (begin == end);
}

// Remove the last path segment from a string range containing a heirarchical path
// and return the pointer past the end of the new path.
char* removeLastSegmentFromRange(char* begin, char* end) {
    if (begin >= end) { return begin; }
    auto pos = end - 1;
    while (pos > begin && *pos != '/') { pos--; }
    return pos;
}

char* Url::removeDotSegmentsFromRange(char* begin, char* end) {

    // Implements https://tools.ietf.org/html/rfc3986#section-5.2.4
    // with in-place manipulation instead of making a new buffer.

    auto pos = begin; // 'input' position.
    auto out = begin; // 'output' position.

    while (pos < end) {
        if (rangeStartsWith(pos, end, "../")) {
            pos += 3;
        } else if (rangeStartsWith(pos, end, "./")) {
            pos += 2;
        } else if (rangeStartsWith(pos, end, "/./")) {
            pos += 2;
        } else if (rangeEquals(pos, end, "/.")) {
            pos += 1;
            *pos = '/';
        } else if (rangeStartsWith(pos, end, "/../")) {
            pos += 3;
            out = removeLastSegmentFromRange(begin, out);
        } else if (rangeEquals(pos, end, "/..")) {
            pos += 2;
            out = removeLastSegmentFromRange(begin, out);
        } else if (rangeIsAll(pos, end, '.')) {
            pos = end;
        } else {
            if (*pos == '/') {
                *out++ = *pos;
                pos++;
            }
            while (pos < end && *pos != '/') {
                *out++ = *pos;
                pos++;
            }
        }
    }

    return out;
}

std::string Url::removeDotSegmentsFromString(std::string path) {
    char* end = removeDotSegmentsFromRange(&path.front(), &path.back() + 1);
    path.resize(end - path.data());
    return path;
}

bool Url::initializeFromString(const std::string& source) {

    // The parsing process roughly follows https://tools.ietf.org/html/rfc1808#section-2.4

    m_string = source;

    size_t start = 0;
    size_t end = m_string.size();

    // Parse the fragment.
    {
        // If there's a '#' in the string, the substring after it to the end is the fragment.
        auto pound = std::min(m_string.find('#', start), end);
        m_parts.fragment.start = std::min(pound + 1, end);
        m_parts.fragment.count = end - m_parts.fragment.start;

        // Remove the '#' and fragment from parsing.
        end = pound;
    }

    // Parse the scheme.
    {
        size_t i = start;
        auto c = m_string[i];

        // A scheme is permitted to contain only alphanumeric characters, '+', '.', and '-'.
        while (i < end && (isalnum(c) || c == '+' || c == '.' || c == '-')) {
            c = m_string[++i];
        }

        // If a scheme is present, it must be followed by a ':'.
        if (c == ':') {
            m_parts.scheme.start = start;
            m_parts.scheme.count = i - start;

            // Remove the scheme and ':' from parsing.
            start = i + 1;

            // Set the absolute URL flag.
            m_flags |= IS_ABSOLUTE;
        }
    }

    // If scheme is 'data', parse as data URI.
    if (m_string.compare(m_parts.scheme.start, m_parts.scheme.count, "data") == 0) {

        // Set the data scheme flag.
        m_flags |= HAS_DATA_SCHEME;

        // A data scheme will be followed by a media type, then either a comma or a base 64 indicator string.
        auto base64Indicator = std::min(m_string.find(";base64", start), end);
        auto comma = std::min(m_string.find(',', start), end);

        // If the base 64 indicator string is found before the comma, set the matching flag.
        if (base64Indicator < comma) {
            m_flags |= HAS_BASE64_DATA;
        }

        // The media type goes from the colon after the scheme up to either the comma or the base 64 string.
        m_parts.media.start = start;
        m_parts.media.count = std::min(base64Indicator, comma) - start;

        // The data section is separated by a comma and goes to the end of the URI.
        start = std::min(comma + 1, end);
        m_parts.data.start = start;
        m_parts.data.count = end - start;

        // We're done!
        return true;
    }

    // Check whether the scheme is 'http', 'https', or 'file' and set appropriate flags.
    if (m_string.compare(m_parts.scheme.start, m_parts.scheme.count, "http") == 0 ||
        m_string.compare(m_parts.scheme.start, m_parts.scheme.count, "https") == 0) {
        m_flags |= HAS_HTTP_SCHEME;
    } else if (m_string.compare(m_parts.scheme.start, m_parts.scheme.count, "file") == 0) {
        m_flags |= HAS_FILE_SCHEME;
    }

    // If '//' is next in the string, then the substring up to the following '/' is the network location.
    if (m_string.compare(start, 2, "//") == 0) {
        start += 2;
        auto slash = std::min(m_string.find('/', start), end);
        m_parts.location.start = start;
        m_parts.location.count = slash - start;

        // Remove the network location from parsing.
        start = slash;
    }

    // Parse the query.
    {
        // If there's a '?' in the remaining string, the substring after it to the end is the query string.
        auto qmark = std::min(m_string.find('?', start), end);
        m_parts.query.start = std::min(qmark + 1, end);
        m_parts.query.count = end - m_parts.query.start;

        // Remove the '?' and query from parsing.
        end = qmark;
    }

    // Parse the parameters.
    {
        // If there's a ';' in the remaining string, the substring after it to the end is the parameters string.
        auto semicolon = std::min(m_string.find(';', start), end);
        m_parts.parameters.start = std::min(semicolon + 1, end);
        m_parts.parameters.count = end - m_parts.parameters.start;

        // Remove the ';' and parameters from parsing.
        end = semicolon;
    }

    // Parse the path. After the preceding steps, the remaining string is the URL path.
    m_parts.path.start = start;
    m_parts.path.count = end - start;

    return true;
}

} // namespace Tangram
