//////////////////////////////////////////////////////////////////////////
// Copyright (C) 2024  Rohit Jairaj Singh (rohit@singh.org.in)          //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program.  If not, see <https://www.gnu.org/licenses/ //
//////////////////////////////////////////////////////////////////////////

#include <json.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

std::string GetFromFile(const std::filesystem::path &path) {
    if (!std::filesystem::is_regular_file(path)) {
        throw std::invalid_argument { "Not a valid file" };
    }
    std::ifstream filestream { path };
    std::stringstream buffer { };
    buffer << filestream.rdbuf();

    std::string content = buffer.str();
    filestream.close();
    return content;
}


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
    std::string newjson = rohit::json::write(*jsonvalue, rohit::json::format::compress);
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
    rohit::json::Value &jsonvalue = *rohit::json::Parse(value);
    EXPECT_TRUE(jsonvalue["Key1"]["key11"].GetString() == "Value1");
    EXPECT_TRUE(jsonvalue["Key1"]["key13"][2].GetInt() == 2);

    std::string newjson = rohit::json::write(jsonvalue, rohit::json::format::compress);
    std::string expectedjson {"{\"Key1\":{\"key11\":\"Value1\",\"key12\":32,\"key13\":[0,1,2,3,4,5,6],\"key14\":true}}"};
    EXPECT_TRUE(newjson == expectedjson);
    std::string newjson1 = rohit::json::write(jsonvalue, rohit::json::format::beautify);
    std::string expectedjson1 {
        "{\n  \"Key1\":\n  {\n    \"key11\": \"Value1\",\n    \"key12\": 32,\n    \"key13\":\n    [\n      0, 1, 2, 3, 4, 5, 6\n    ],\n    \"key14\": true\n  }\n}"
    };
    EXPECT_TRUE(newjson1 == expectedjson1);
    std::string newjson2 = rohit::json::write(jsonvalue, rohit::json::format::beautify_vertical);
    std::string expectedjson2 {
        "{\n  \"Key1\":\n  {\n    \"key11\": \"Value1\",\n    \"key12\": 32,\n    \"key13\":\n    [\n      0,\n      1,\n      2,\n      3,\n      4,\n      5,\n      6\n    ],\n    \"key14\": true\n  }\n}"
    };
    EXPECT_TRUE(newjson2 == expectedjson2);

    rohit::json::Value &jsonvalue1 = *rohit::json::Parse(newjson1);
    EXPECT_TRUE(jsonvalue == jsonvalue1);
}

TEST(JSONTest, JSONFile) {
    const std::string jsonstr = GetFromFile("./resources/simpleprofile.json");
    auto json = rohit::json::Parse(jsonstr);
    std::vector<std::string> courses { "Math", "Physics", "Chemistry" };
    auto jsoncourses = (*json)["courses"].GetStringVector(false);
    EXPECT_TRUE(courses == jsoncourses);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}