#include "single_file_test.hpp"
#include "lust/container/simple_string.hpp"

void entry() {
    using namespace lust;

    // Test default constructor
    simple_string str1;
    TEST_CHECK_OK_MSG(str1.is_empty(), "Default constructor failed, string is not empty.");

    // Test constructor with const char*
    simple_string str2("Hello");
    TEST_CHECK_OK_MSG(!str2.is_empty(), "Constructor with const char* failed, string is empty.");
    TEST_CHECK_OK_MSG(str2.length() == 5, "Constructor with const char* failed, length is not correct.");
    TEST_CHECK_OK_MSG(str2 == "Hello", "Constructor with const char* failed, content is not correct.");

    // Test constructor with std::string_view
    simple_string str3(std::string_view("World"));
    TEST_CHECK_OK_MSG(!str3.is_empty(), "Constructor with std::string_view failed, string is empty.");
    TEST_CHECK_OK_MSG(str3.length() == 5, "Constructor with std::string_view failed, length is not correct.");
    TEST_CHECK_OK_MSG(str3 == "World", "Constructor with std::string_view failed, content is not correct.");

    // Test copy constructor
    simple_string str4(str2);
    TEST_CHECK_OK_MSG(str4 == str2, "Copy constructor failed, content is not correct.");
    TEST_CHECK_OK_MSG(str4.length() == str2.length(), "Copy constructor failed, length is not correct.");

    // Test move constructor
    simple_string str5(std::move(str3));
    TEST_CHECK_OK_MSG(str5 == "World", "Move constructor failed, content is not correct.");
    TEST_CHECK_OK_MSG(str3.is_empty(), "Move constructor failed, original string is not empty.");

    // Test copy assignment
    simple_string str6;
    str6 = str2;
    TEST_CHECK_OK_MSG(str6 == str2, "Copy assignment failed, content is not correct.");
    TEST_CHECK_OK_MSG(str6.length() == str2.length(), "Copy assignment failed, length is not correct.");

    // Test move assignment
    simple_string str7;
    str7 = std::move(str2);
    TEST_CHECK_OK_MSG(str7 == "Hello", "Move assignment failed, content is not correct.");
    TEST_CHECK_OK_MSG(str2.is_empty(), "Move assignment failed, original string is not empty.");

    // Test append(const char*)
    simple_string str8("Good");
    str8.append(" Morning");
    TEST_CHECK_OK_MSG(str8 == "Good Morning", "Append with const char* failed, content is not correct.");

    // Test append(std::string_view)
    simple_string str9("Good");
    str9.append(std::string_view(" Evening"));
    TEST_CHECK_OK_MSG(str9 == "Good Evening", "Append with std::string_view failed, content is not correct.");

    // Test operator+= with const char*
    simple_string str10("Good");
    str10 += " Night";
    TEST_CHECK_OK_MSG(str10 == "Good Night", "Operator+= with const char* failed, content is not correct.");

    // Test operator+= with std::string_view
    simple_string str11("Good");
    str11 += std::string_view(" Afternoon");
    TEST_CHECK_OK_MSG(str11 == "Good Afternoon", "Operator+= with std::string_view failed, content is not correct.");

    // Test operator+= with simple_string
    simple_string str12("Hello");
    simple_string str13(" World");
    str12 += str13;
    TEST_CHECK_OK_MSG(str12 == "Hello World", "Operator+= with simple_string failed, content is not correct.");

    // Test swap
    simple_string str14("First");
    simple_string str15("Second");
    str14.swap(str15);
    TEST_CHECK_OK_MSG(str14 == "Second", "Swap failed, str14 content is not correct.");
    TEST_CHECK_OK_MSG(str15 == "First", "Swap failed, str15 content is not correct.");
}
