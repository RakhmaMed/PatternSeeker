#ifndef PATTERN_SEEKER_H
#define PATTERN_SEEKER_H

#include <string_view>
#include <string>
#include <optional>

#include <cstdint>
#include <cctype>
#include <cerrno>
#include <cstdlib>

namespace PatterSeekerNS
{

// MoveMode allows you to move the pointer before or after the desired data.
// By default, the pointer doesn't move.
enum MoveMode
{
    none,
    move_before,
    move_after,
};

// PatternSeeker is a class that is easy to use for parsing small strings with a predefined pattern.
// This class is just a display of the string passed in the constructor.
// Because of this, the object is very lightweight and can be copied at zero cost.
// Therefore, the responsibility for the lifetime of the original string falls on the developer.
// PatternSeeker also allows you to parse Json and XML.
// The author strongly recommends using appropriate parsers, however,
// this class is convenient because it doesn't make a copy of the data
// and allows you to easily and quickly parse a large structure and not use complex and heavy regular expressions.
// For better performance, this class offers the user to decide for himself when to move the pointer and when not.
class PatternSeeker
{
private:
    static constexpr const char* EMPTY_STR = "";
    static inline const std::string DOUBLE_QUOTE = "\"";

    std::string_view m_str;
    const char* m_originalPointer;

    PatternSeeker(std::string_view str, const char* originalPointer)
        : m_str(str.data() ? str : EMPTY_STR)
        , m_originalPointer(originalPointer)
    {}

    PatternSeeker()
        : m_str(EMPTY_STR)
        , m_originalPointer(EMPTY_STR)
    {}

public:
    PatternSeeker(std::string_view str)
        : m_str(str.data() ? str : EMPTY_STR)
        , m_originalPointer(m_str.data())
    {}

    // Returns the size of the displayed string
    size_t size() const
    {
        return m_str.size();
    }

    // Checks the string for emptiness
    bool isEmpty() const
    {
        return size() == 0;
    }

    // Checking for non-emptiness (for better readability)
    bool isNotEmpty() const
    {
        return size() != 0;
    }

    // Returns the string_view of the visible part
    std::string_view to_string_view() const
    {
        return m_str;
    }

    // Returns the string of the visible part
    std::string to_string() const
    {
        return std::string{ m_str.data(), m_str.size() };
    }

    // check what next and move the pointer after `expected`
    // return true if found `expected`
    bool expect(std::string_view expected)
    {
        if (m_str.starts_with(expected))
        {
            m_str.remove_prefix(expected.size());
            return true;
        }

        return false;
    }

    // Check what next and don't move the pointer
    bool startsWith(std::string_view expected) const
    {
        return m_str.starts_with(expected);
    }

    // Find the `expected` string and move the pointer after `expected`
    bool to(std::string_view expected, MoveMode mode=none)
    {
        const size_t pos = m_str.find(expected);
        if (pos == std::string_view::npos)
        {
            return false;
        }

        switch (mode)
        {
        case move_before:
            m_str.remove_prefix(pos);
            break;
        case move_after:
            m_str.remove_prefix(pos + expected.size());
            break;
        case none:
            // don't move
            break;
        }

        return true;
    }

    // Extract data `from` and `to` the desired strings.
    PatternSeeker extract(std::string_view from, std::string_view to, MoveMode mode=none)
    {
        auto startIt = m_str.find(from);
        if (startIt == std::string::npos)
        {
            return {};
        }
        startIt += from.size();

        const auto endIt = m_str.find(to, startIt);
        if (endIt == std::string::npos) {
            return {};
        }

        auto substr = m_str.substr(startIt, endIt - startIt);

        switch (mode)
        {
        case move_before:
            m_str.remove_prefix(startIt - from.size());
            break;
        case move_after:
            m_str.remove_prefix(endIt + to.size());
            break;
        case none:
            // don't move
            break;
        default:
            break;
        }

        return PatternSeeker(substr, m_originalPointer);
    }

    // Extract data from current position and `to` the desired strings.
    PatternSeeker extract(std::string_view to, MoveMode mode=none)
    {
        const auto endIt = m_str.find(to);
        if (endIt == std::string::npos)
        {
            return {};
        }

        auto substr = m_str.substr(0, endIt);

        switch (mode)
        {
        case move_before:
            m_str.remove_prefix(endIt);
            break;
        case move_after:
            m_str.remove_prefix(endIt + to.size());
            break;
        case none:
            // don't move
            break;
        default:
            break;
        }

        return PatternSeeker(substr, m_originalPointer);
    }

    // Extract data from current position `to` the desired symbols.
    PatternSeeker extractUntilOneOf(std::string_view to, MoveMode mode=none)
    {
        const auto endIt = m_str.find_first_of(to);
        if (endIt == std::string::npos)
        {
            return {};
        }

        auto substr = m_str.substr(0, endIt);

        if (mode == move_after)
            m_str.remove_prefix(endIt + 1);

        return PatternSeeker(substr, m_originalPointer);
    }

    // to avoid implicit convertion
    PatternSeeker extract(char) = delete;

    // Extracts data from one character to another.
    // However, if another `start` character is founded during the search for the `end` character,
    // the search continues until the corresponding `end` character is found.
    // It is convenient to search for objects in curly or square brackets.
    PatternSeeker extract(char start, char end, MoveMode mode=none)
    {
        size_t startIndex = m_str.find(start);
        if (startIndex == std::string::npos)
            return {};

        size_t i = startIndex + 1;
        int depth = 1;
        while (i < m_str.size())
        {
            char ch = m_str[i++];
            if (ch == start) ++depth;
            else if (ch == end) {
                --depth;
                if (depth == 0) break;
            }
        }
        if (depth != 0)
            return {};

        auto substr = m_str.substr(startIndex, i - startIndex);

        switch (mode)
        {
        case move_before:
            m_str.remove_prefix(startIndex);
            break;
        case move_after:
            m_str.remove_prefix(i);
            break;
        case none:
            break;
        }

        return PatternSeeker{substr, m_originalPointer};
    }

    // Extracts the required number of characters
    PatternSeeker extract(size_t size, MoveMode mode=none)
    {
        auto substr = m_str.substr(0, size);

        if (mode == move_after)
            m_str.remove_prefix(size);

        return PatternSeeker{substr, m_originalPointer};
    }

    // to avoid implicit convertion
    void skip(char n) = delete;

    // Move the pointer and skip `n` elements
    void skip(size_t n)
    {
        m_str.remove_prefix(n);
    }

    // Parses an unsigned number and shifts the pointer.
    // An empty boost::optional is returned on failure
    std::optional<uint64_t> takeUInt64()
    {
        errno = 0; // thread safe
        char* end = nullptr;
        const auto result = std::strtoull(m_str.data(), &end, 10);
        bool isError = errno || end == m_str.data();
        m_str.remove_prefix(end - m_str.data());
        if (isError)
            return {};
        return result;
    }

    // An auxiliary number parsing method
    // that takes the default value and returns it in case of failure.
    uint64_t takeUInt64(uint64_t def)
    {
        auto res = takeUInt64();
        if (!res)
            return def;
        return *res;
    }

    // Parses a signed number and shifts the pointer.
    // An empty boost::optional is returned on failure
    std::optional<int64_t> takeInt64()
    {
        errno = 0; // thread safe
        char* end = nullptr;
        const auto result = std::strtoll(m_str.data(), &end, 10);
        bool isError = errno || end == m_str.data();
        m_str.remove_prefix(end - m_str.data());
        if (isError)
            return {};
        return result;
    }

    // An auxiliary number parsing method
    // that takes the default value and returns it in case of failure.
    int64_t takeInt64(int64_t def)
    {
        auto res = takeInt64();
        if (!res)
            return def;
        return *res;
    }

    // Removes all whitespace characters
    void skipWhiteSpaces()
    {
        while (m_str.size() && std::isspace(static_cast<unsigned char>(m_str[0]))) {
            m_str.remove_prefix(1);
        }
    }

    // Returns Json data by its name, whether it is a string, a number, an array, or a new object.
    PatternSeeker getJsonProp(std::string prop)
    {
        auto copy = *this;
        if (!copy.to(DOUBLE_QUOTE + prop + DOUBLE_QUOTE, move_after))
            return {};

        copy.skipWhiteSpaces();
        if (!copy.expect(":"))
            return {};

        copy.skipWhiteSpaces();
        if (copy.expect(DOUBLE_QUOTE))
            return copy.extract(DOUBLE_QUOTE);

        if (copy.startsWith("["))
            return copy.extract('[', ']');

        if (copy.startsWith("{"))
            return copy.extract('{', '}');

        return copy.extractUntilOneOf(", \r\n]}");
    }

    // Returns the contents of the XML tag
    PatternSeeker getXmlTagBody(std::string prop, MoveMode mode=none)
    {
        auto res = getXmlTag(prop, mode);
        if (res.isEmpty())
            return {};
        auto startPos = res.m_str.find('>') + 1;
        auto endPos = res.m_str.rfind("</" + prop + ">");
        if (endPos == std::string::npos)
            return {};
        return res.m_str.substr(startPos, endPos - startPos);
    }

    // Returns the entire tag, including the tag name and its attributes
    PatternSeeker getXmlTag(std::string prop, MoveMode mode=none)
    {
        auto startTag = "<" + prop;
        size_t startPos = m_str.find(startTag);
        if (startPos == std::string::npos)
            return {};

        auto endTag = "</" + prop + ">";
        size_t endPos = m_str.find(endTag, startPos + startTag.size());
        if (endPos == std::string::npos)
            return {};

        auto substr = m_str.substr(startPos, endPos + endTag.size() - startPos);

        switch (mode)
        {
        case move_before:
            m_str.remove_prefix(startPos);
            break;
        case move_after:
            m_str.remove_prefix(endPos + endTag.size());
            break;
        default:
            break;
        }

        return PatternSeeker{substr, m_originalPointer};
    }

    // Returns the contents of the XML attribute
    PatternSeeker getXmlAttr(std::string prop)
    {
        auto copy = *this;
        if (!copy.to(prop, move_after))
            return {};
        copy.skipWhiteSpaces();
        copy.expect("=");
        copy.skipWhiteSpaces();
        return copy.extract(DOUBLE_QUOTE, DOUBLE_QUOTE);
    }

    // useful to know where a string starts
    size_t getOriginalPosition()
    {
        return m_str.data() - m_originalPointer;
    }

    // useful to know how much we've moved on.
    // For example, buffer.commit(offset)
    size_t getOffset()
    {
        return m_str.data() - m_originalPointer;
    }

    // output to the stream
    friend std::ostream& operator<<(std::ostream& oss, const PatternSeeker& parser) {
        return oss << parser.to_string_view();
    }
};

}

#endif
