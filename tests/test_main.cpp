#include "../PatternSeeker.hpp"

#include <iostream>
#include <cassert>
#include <string>

using namespace PatterSeekerNS;

void test_basic_operations() {
    std::cout << "Testing basic operations..." << std::endl;
    
    PatternSeeker ps("Hello, World!");
    assert(ps.size() == 13);
    assert(!ps.isEmpty());
    assert(ps.isNotEmpty());
    assert(ps.to_string() == "Hello, World!");
    
    std::cout << "  ✓ Basic operations passed" << std::endl;
}

void test_expect() {
    std::cout << "Testing expect..." << std::endl;
    
    PatternSeeker ps("Hello, World!");
    assert(ps.expect("Hello"));
    assert(ps.to_string() == ", World!");
    assert(!ps.expect("Goodbye"));
    
    std::cout << "  ✓ Expect passed" << std::endl;
}

void test_starts_with() {
    std::cout << "Testing startsWith..." << std::endl;
    
    PatternSeeker ps("Hello, World!");
    assert(ps.startsWith("Hello"));
    assert(!ps.startsWith("World"));
    assert(ps.to_string() == "Hello, World!"); // Should not modify
    
    std::cout << "  ✓ StartsWith passed" << std::endl;
}

void test_to() {
    std::cout << "Testing to..." << std::endl;
    
    PatternSeeker ps("Hello, World!");
    assert(ps.to(",", move_after));
    assert(ps.to_string() == " World!");
    
    PatternSeeker ps2("Hello, World!");
    assert(ps2.to("World", move_before));
    assert(ps2.to_string() == "World!");
    
    std::cout << "  ✓ To passed" << std::endl;
}

void test_extract() {
    std::cout << "Testing extract..." << std::endl;
    
    PatternSeeker ps("Hello, <name>World</name>!");
    auto extracted = ps.extract("<name>", "</name>");
    assert(extracted.to_string() == "World");
    
    PatternSeeker ps2("Hello, World!");
    auto extracted2 = ps2.extract("World");
    assert(extracted2.to_string() == "Hello, ");
    
    std::cout << "  ✓ Extract passed" << std::endl;
}

void test_extract_brackets() {
    std::cout << "Testing extract with brackets..." << std::endl;
    
    PatternSeeker ps("{\"key\": {\"nested\": \"value\"}}");
    auto extracted = ps.extract('{', '}');
    assert(extracted.to_string() == "{\"key\": {\"nested\": \"value\"}}");
    
    std::cout << "  ✓ Extract brackets passed" << std::endl;
}

void test_take_uint64() {
    std::cout << "Testing takeUInt64..." << std::endl;
    
    PatternSeeker ps("12345 remainder");
    auto num = ps.takeUInt64();
    assert(num.has_value());
    assert(*num == 12345);
    assert(ps.to_string() == " remainder");
    
    PatternSeeker ps2("not_a_number");
    auto num2 = ps2.takeUInt64();
    assert(!num2.has_value());
    
    std::cout << "  ✓ TakeUInt64 passed" << std::endl;
}

void test_take_int64() {
    std::cout << "Testing takeInt64..." << std::endl;
    
    PatternSeeker ps("-12345 remainder");
    auto num = ps.takeInt64();
    assert(num.has_value());
    assert(*num == -12345);
    assert(ps.to_string() == " remainder");
    
    PatternSeeker ps2("not_a_number");
    auto num2 = ps2.takeInt64();
    assert(!num2.has_value());
    
    std::cout << "  ✓ TakeInt64 passed" << std::endl;
}

void test_skip_whitespaces() {
    std::cout << "Testing skipWhiteSpaces..." << std::endl;
    
    PatternSeeker ps("   \t\n  Hello");
    ps.skipWhiteSpaces();
    assert(ps.to_string() == "Hello");
    
    std::cout << "  ✓ SkipWhiteSpaces passed" << std::endl;
}

void test_json() {
    std::cout << "Testing JSON operations..." << std::endl;
    
    PatternSeeker ps(R"({"name": "John", "age": 30, "array": [1,2,3], "obj": {"nested": "value"}})");
    
    auto name = ps.getJsonProp("name");
    assert(name.to_string() == "John");
    
    auto age = ps.getJsonProp("age");
    assert(age.to_string() == "30");
    
    auto arr = ps.getJsonProp("array");
    assert(arr.to_string() == "[1,2,3]");
    
    auto obj = ps.getJsonProp("obj");
    assert(obj.to_string() == "{\"nested\": \"value\"}");
    
    std::cout << "  ✓ JSON operations passed" << std::endl;
}

void test_xml() {
    std::cout << "Testing XML operations..." << std::endl;
    
    PatternSeeker ps("<root><name>John</name><age>30</age></root>");
    
    auto name = ps.getXmlTagBody("name");
    assert(name.to_string() == "John");
    
    auto nameTag = ps.getXmlTag("name");
    assert(nameTag.to_string() == "<name>John</name>");
    
    std::cout << "  ✓ XML operations passed" << std::endl;
}

void test_xml_attributes() {
    std::cout << "Testing XML attributes..." << std::endl;
    
    PatternSeeker ps(R"(<tag id="123" class="example">content</tag>)");
    
    auto id = ps.getXmlAttr("id");
    assert(id.to_string() == "123");
    
    auto cls = ps.getXmlAttr("class");
    assert(cls.to_string() == "example");
    
    std::cout << "  ✓ XML attributes passed" << std::endl;
}

void test_offset() {
    std::cout << "Testing offset operations..." << std::endl;
    
    std::string str = "Hello, World!";
    PatternSeeker ps(str);
    
    assert(ps.getOriginalPosition() == 0);
    assert(ps.getOffset() == 0);
    
    ps.skip(size_t(7));
    assert(ps.getOffset() == 7);
    
    std::cout << "  ✓ Offset operations passed" << std::endl;
}

int main() {
    std::cout << "\n=== PatternSeeker Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        test_basic_operations();
        test_expect();
        test_starts_with();
        test_to();
        test_extract();
        test_extract_brackets();
        test_take_uint64();
        test_take_int64();
        test_skip_whitespaces();
        test_json();
        test_xml();
        test_xml_attributes();
        test_offset();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}