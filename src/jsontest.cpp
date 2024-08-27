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

#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
const std::vector<std::string> samplejsonlist {
    "{\n"
    "    \"Key1\" : {\n"
    "        \"key11\" : \"Value1\",\n"
    "        \"key12\" : 32,\n"
    "        \"key13\" : [0, 1, 2, 3, 4, 5, 6],\n"
    "        \"key14\" :true,\n"
    "    }\n"
    "}\n",
    "[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]",
    "[0, 1, 2, [\"Test1\", \"Test2\", \"Test3\"], 4, 5, 6]",
    "{\"zero\": 0, \"one\": 1, \"two\": 2, \"three\": 3, \"four\": 4, \"five\": 5, \"six\": 6, \"seven\": 7, \"eight\": 8, \"nine\": 9}",
    "{\"0\": 0, \"1\": 1, \"2\": 2, \"3\": 3, \"4\": 4, \"5\": 5, \"6\": 6, \"7\": 7, \"8\": 8, \"9\": 9}",
    "{\n"
    "    \"Key1\" : {\n"
    "        \"key16\" : [ ],\n"
    "        \"key15\" : { },\n"
    "        \"key11\" : \"Value1\",\n"
    "        \"key12\" : 32,\n"
    "        \"key13\" : [0, 1, 2, 3, 4, 5, 6],\n"
    "        \"key14\" :true,\n"
    "    }\n"
    "}\n",
};

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
        auto jsonref = rohit::json::Parse(json);

        auto newvalue = jsonref.GetString();
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
        auto jsonref = rohit::json::Parse(json);

        auto newvalue = jsonref.GetBool();
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
        auto jsonref = rohit::json::Parse(json);
        
        auto newvalue = jsonref.GetInt();
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
        auto jsonref = rohit::json::Parse(json);
        
        auto newvalue = jsonref.GetFloat();
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



TEST(JSONTest, ArrayTest) {
    const std::string &value = samplejsonlist[1];
    auto json = rohit::json::Parse(value);
    EXPECT_TRUE(json[0].GetInt() == 0);
    EXPECT_TRUE(json[2].GetInt() == 2);
    EXPECT_TRUE(json[6].GetInt() == 6);

    const std::string &value1 = samplejsonlist[2];
    auto json1 = rohit::json::Parse(value1);
    EXPECT_TRUE(json1[3][0].GetString() == "Test1");
    EXPECT_TRUE(json1[3][2].GetString() == "Test3");

    const std::string expectedjson { "[0,1,2,3,4,5,6,7,8,9]"};
    std::string newjson = json.write(rohit::json::format::compress);
    EXPECT_TRUE(newjson == expectedjson);
}

TEST(JSONTest, ObjectTest) {
    const std::string &value = samplejsonlist[0];
    auto jsonref = rohit::json::Parse(value);
    EXPECT_TRUE(jsonref["Key1"]["key11"].GetString() == "Value1");
    EXPECT_TRUE(jsonref["Key1"]["key13"][2].GetInt() == 2);

    std::string newjson = jsonref.write(rohit::json::format::compress);
    std::string expectedjson {"{\"Key1\":{\"key11\":\"Value1\",\"key12\":32,\"key13\":[0,1,2,3,4,5,6],\"key14\":true}}"};
    EXPECT_TRUE(newjson == expectedjson);
    std::string newjson1 = jsonref.write(rohit::json::format::beautify);
    std::string expectedjson1 {
        "{\n  \"Key1\":\n  {\n    \"key11\": \"Value1\",\n    \"key12\": 32,\n    \"key13\":\n    [\n      0, 1, 2, 3, 4, 5, 6\n    ],\n    \"key14\": true\n  }\n}"
    };
    EXPECT_TRUE(newjson1 == expectedjson1);
    std::string newjson2 = jsonref.write(rohit::json::format::beautify_vertical);
    std::string expectedjson2 {
        "{\n  \"Key1\":\n  {\n    \"key11\": \"Value1\",\n    \"key12\": 32,\n    \"key13\":\n    [\n      0,\n      1,\n      2,\n      3,\n      4,\n      5,\n      6\n    ],\n    \"key14\": true\n  }\n}"
    };
    EXPECT_TRUE(newjson2 == expectedjson2);

    auto jsonref1 = rohit::json::Parse(newjson1);
    EXPECT_TRUE(jsonref == jsonref1);
}

TEST(JSONTest, JSONFile) {
    const std::string jsonstr = GetFromFile("./resources/simpleprofile.json");
    auto json = rohit::json::Parse(jsonstr);
    std::vector<std::string> courses { "Math", "Physics", "Chemistry" };
    auto jsoncourses = json["courses"].GetStringVector(false);
    EXPECT_TRUE(courses == jsoncourses);
}

constexpr int ConstExprJSON() {
    const std::string value { "[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"};
    auto json = rohit::json::Parse(value);
    int int_value = json[2].GetInt();
    return int_value;
}

TEST(JSONTest, ConstExpr) {
    int value = ConstExprJSON();
    EXPECT_TRUE(value == 2);
}

TEST(JSONTest, JSONQuery) {
    auto json = rohit::json::Parse(samplejsonlist[5]);
    auto &jsonvalue = json.Query(std::string {"/Key1/key12"});
    EXPECT_TRUE(jsonvalue.IsInteger() && jsonvalue.GetInt() == 32);

    auto &jsonvalue1 = json.Query(std::string {"/Key1/errr"});
    EXPECT_TRUE(jsonvalue1.IsError());

    auto &jsonvalue2 = json.Query(std::string {"/Key1/key14"});
    EXPECT_TRUE(jsonvalue2.IsBool() && jsonvalue2.GetBool());

    auto &jsonvalue3 = json.Query(std::string {"/Key1/key11"});
    EXPECT_TRUE(jsonvalue3.IsString() && jsonvalue3.GetString() == "Value1");

    auto &jsonvalue4 = json.Query(std::string {"/Key1/key13"});
    EXPECT_TRUE(jsonvalue4.IsArray());

    auto &jsonvalue5 = json.Query(std::string {"/Key1/key15"});
    EXPECT_TRUE(jsonvalue5.IsObject() && jsonvalue5.empty());

    auto &jsonvalue6 = json.Query(std::string {"/Key1/key16"});
    EXPECT_TRUE(jsonvalue6.IsArray() && jsonvalue6.empty());
}

TEST(JSONTest, ArrayIterator) {
    auto json = rohit::json::Parse(samplejsonlist[1]);
    int index { 0 };
    for(auto &val: json) {
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == index++);
    }

    for(auto begin { std::rbegin(json) }; begin != std::rend(json); ++begin) {
        auto &val = *begin;
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == --index);
    }
    
    // Const iterator
    const auto json1 = rohit::json::Parse(samplejsonlist[1]);
    for(auto &val: json1) {
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == index++);
    }

    for(auto begin { std::rbegin(json1) }; begin != std::rend(json1); ++begin) {
        auto &val = *begin;
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == --index);
    }
}

TEST(JSONTest, ObjectIterator) {
    auto json = rohit::json::Parse(samplejsonlist[4]);
    int index { 0 };
    for(auto &member: json) {
        auto &val = member.GetValue();
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == index++);
    }

    for(auto itr { std::rbegin(json) }; itr != std::rend(json); ++itr) {
        auto &val = itr->GetValue();
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == --index);
    }
    
    // Const iterator
    const auto json1 = rohit::json::Parse(samplejsonlist[4]);
    for(auto &member: json1) {
        auto &val = member.GetValue();
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == index++);
    }

    for(auto itr { std::rbegin(json1) }; itr != std::rend(json1); ++itr) {
        auto &val = itr->GetValue();
        EXPECT_TRUE(val.IsInteger() && val.GetInt() == --index);
    }
}

TEST(JSONTest, EmptyTest) {
    auto json = rohit::json::Parse(samplejsonlist[5]);
    //auto &arr = json[.Query("/Key1/key16")];
    auto &arr = json["Key1"]["key16"];
    EXPECT_TRUE(arr.IsArray() && arr.empty());

    auto &obj = json.Query("/Key1/key15");
    EXPECT_TRUE(obj.IsObject() && obj.empty());
}

TEST(StreamTest, ExceptionTest) {
    std::string teststr { "Test" };
    rohit::json::Stream str { teststr };
    ASSERT_THROW(--str, rohit::json::StreamUnderflowException);
    EXPECT_TRUE(*str == 'T');
    EXPECT_TRUE(str.index() == 0);
    EXPECT_TRUE(str.capacity() == 4);
    EXPECT_TRUE(str.remaining_buffer() == 4);
    ++str;
    EXPECT_TRUE(*str == 'e');
    EXPECT_TRUE(str.index() == 1);
    EXPECT_TRUE(str.capacity() == 4);
    EXPECT_TRUE(str.remaining_buffer() == 3);
    ++str;
    EXPECT_TRUE(*str == 's');
    EXPECT_TRUE(str.index() == 2);
    EXPECT_TRUE(str.capacity() == 4);
    EXPECT_TRUE(str.remaining_buffer() == 2);
    ++str;
    EXPECT_TRUE(*str == 't');
    EXPECT_TRUE(str.index() == 3);
    EXPECT_TRUE(str.capacity() == 4);
    EXPECT_TRUE(str.remaining_buffer() == 1);
    ++str;
    EXPECT_TRUE(str.index() == 4);
    EXPECT_TRUE(str.capacity() == 4);
    EXPECT_TRUE(str.remaining_buffer() == 0);
    ASSERT_THROW(*str, rohit::json::StreamOverflowException);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}