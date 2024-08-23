/////////////////////////////////////////////////////////////////////////////////////////////
// Author: Rohit Jairaj Singh (rohit@singh.org.in)                                         //
// LICENSE: GPL-3.0                                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////

#include <json.h>
#include <gtest/gtest.h>

bool StringCreateAndMatch(const std::string &value) {
    std::string json { "\"" };
    json += value;
    json += "\"";
    try {
        rohit::json::Value *jsonvalue = rohit::json::Parse(json);

        auto newvalue = jsonvalue->GetString();
        std::string expanded { };
        rohit::json::String::EscapeString(newvalue, expanded);
        return value == expanded;
    }
    catch(rohit::json::JsonParseException &e) {
        std::cout << e.what() << std::endl;
    }
    return false;
}

bool BoolCreateAndMatch(const std::string &json, bool expected) {
    try {
        rohit::json::Value *jsonvalue = rohit::json::Parse(json);

        auto newvalue = jsonvalue->GetBool();
        return newvalue == expected;
    }
    catch(rohit::json::JsonParseException &e) {
        std::cout << e.to_string() << std::endl;
    }
    return false;
}

bool IntegerCreateAndMatch(int value) {
    try {
        std::string json = std::to_string(value);
        auto jsonvalue = rohit::json::Parse(json);
        
        auto newvalue = jsonvalue->GetInt();
        return newvalue == value;
    }
    catch(rohit::json::JsonParseException &e) {
        std::cout << e.what() << std::endl;
    }
    return false;
}

bool FloatCreateAndMatch(double value) {
    try {
        std::string json = std::to_string(value);
        auto jsonvalue = rohit::json::Parse(json);
        
        auto newvalue = jsonvalue->GetFloat();
        return newvalue == value;
    }
    catch(rohit::json::JsonParseException &e) {
        std::cout << e.what() << std::endl;
    }
    return false;
}

TEST(JSONTest, BoolTest) {
    const std::vector<std::pair<std::string, bool>> values {
        {"true", true},
        {"false", false}
    };

    for (auto &value: values) {
        EXPECT_TRUE(BoolCreateAndMatch(value.first, value.second));
    }
}

TEST(JSONTest, IntegerTest) {
    const std::vector<int> values {1, -1, 0, 2, -2, 65537, -65537, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

    for (auto value: values) {
        EXPECT_TRUE(IntegerCreateAndMatch(value));
    }
}

TEST(JSONTest, FloatTest) {
    const std::vector<double> values {1.1, -1.2, 0, 2.3, -2.4, 65537.1, -65537.2, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

    for (auto value: values) {
        EXPECT_TRUE(FloatCreateAndMatch(value));
    }
}

TEST(JSONTest, StringTest) {
    const std::vector<std::string> values {
        "Test \\nescape",
        "Test",
        "This is a test",
        "\\\"Test \\nescape\\\"",
        "Testing String is a good exercise"
    };

    for (auto &value: values)
        EXPECT_TRUE(StringCreateAndMatch(value));
}

TEST(JSONTest, JsonArray) {
    const std::string value { "[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"};
    rohit::json::Value *jsonvalue = rohit::json::Parse(value);
    EXPECT_TRUE((*jsonvalue)[0].GetInt() == 0);
    EXPECT_TRUE((*jsonvalue)[2].GetInt() == 2);
    EXPECT_TRUE((*jsonvalue)[6].GetInt() == 6);

    const std::string value1 { "[0, 1, 2, [\"Test1\", \"Test2\", \"Test3\"], 4, 5, 6]"};
    rohit::json::Value *jsonvalue1 = rohit::json::Parse(value1);
    EXPECT_TRUE((*jsonvalue1)[3][0].GetString() == "Test1");
    EXPECT_TRUE((*jsonvalue1)[3][2].GetString() == "Test3");

    const std::string expectedjson { "[0,1,2,3,4,5,6,7,8,9]"};
    std::string newjson { };
    jsonvalue->write(newjson);
    EXPECT_TRUE(newjson == expectedjson);
}

TEST(JSONTest, JsonObject) {
    const std::string value {
        "{\n"
        "    \"Key1\" : {\n"
        "        \"key11\" : \"Value1\",\n"
        "        \"key12\" : 32,\n"
        "        \"key13\" : [0, 1, 2, 3, 4, 5, 6],\n"
        "        \"key14\" :true,\n"
        "    }\n"
        "}\n"
    };
    rohit::json::Value *jsonvalueptr = rohit::json::Parse(value);
    rohit::json::Value &jsonvalue = *jsonvalueptr;
    EXPECT_TRUE(jsonvalue["Key1"]["key11"].GetString() == "Value1");
    EXPECT_TRUE(jsonvalue["Key1"]["key13"][2].GetInt() == 2);

    std::string newjson { };
    jsonvalue.write(newjson);
    std::string expectedjson { "{Key1:{key11:\"Value1\",key12:32,key13:[0,1,2,3,4,5,6],key14:true}}"};
    EXPECT_TRUE(newjson == expectedjson);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}