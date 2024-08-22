/////////////////////////////////////////////////////////////////////////////////////////////
// Author: Rohit Jairaj Singh (rohit@singh.org.in)                                         //
// LICENSE: GPL-3.0                                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////

#include <json.h>
#include <gtest/gtest.h>

rohit::json::Value *Parse(const std::string &json) {
    try {
        return rohit::json::Value::Parse(json);
    }
    catch(rohit::json::JsonParseException &e) {
        std::cout << e.to_string(json.c_str()) << std::endl;
    }
    return nullptr;
}

std::string ExpandEscape(const std::string &original) {
    std::string newstr { };
    auto itr = std::begin(original);
    while(itr != std::end(original)) {
        switch(*itr) {
        case '"':
            newstr.push_back('\\');
            newstr.push_back('"');
            break;
        case '\\':
            newstr.push_back('\\');
            newstr.push_back('\\');
            break;
        case '\b':
            newstr.push_back('\\');
            newstr.push_back('b');
            break;
        case '\f':
            newstr.push_back('\\');
            newstr.push_back('f');
            break;
        case '\n':
            newstr.push_back('\\');
            newstr.push_back('n');
            break;
        case '\r':
            newstr.push_back('\\');
            newstr.push_back('r');
            break;
        case '\t':
            newstr.push_back('\\');
            newstr.push_back('t');
            break;
        default:
            newstr.push_back(*itr);
            break;
        }
        itr = std::next(itr);
    }
    return newstr;
}

bool StringCreateAndMatch(const std::string &value) {
    std::string json { "\"" };
    json += value;
    json += "\"";

    rohit::json::Value *jsonvalue = Parse(json);

    auto newvalue = jsonvalue->GetString();
    auto expanded = ExpandEscape(newvalue);
    return value == expanded;
}

TEST(JSONTest, StringTest) {
    const std::vector<std::string> values {
        "Test",
        "This is a test",
        "Test \\nescape",
        "\\\"Test \\nescape\\\"",
        "Testing String is a good exercise"
    };

    for (auto &value: values)
        EXPECT_TRUE(StringCreateAndMatch(value));
}

TEST(JSONTest, JsonArray) {
    const std::string value { "[1, 2, 3, 4, 5, 6]"};
    rohit::json::Value *jsonvalueptr = Parse(value);
    rohit::json::Value &jsonvalue = *jsonvalueptr;
    EXPECT_TRUE(jsonvalue[2].GetInt() == 3);
}

TEST(JSONTest, JsonObject) {
    const std::string value {
        "{\n"
        "    \"Key1\" : {\n"
        "        \"key11\" : \"Value1\",\n"
        "        \"key12\" : 32,\n"
        "        \"key13\" : [1, 2, 3, 4, 5, 6],\n"
        "    }\n"
        "}\n"
    };
    rohit::json::Value *jsonvalueptr = Parse(value);
    rohit::json::Value &jsonvalue = *jsonvalueptr;
    EXPECT_TRUE(jsonvalue["Key1"]["key11"].GetString() == "Value1");
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}