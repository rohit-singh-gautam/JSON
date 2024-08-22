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

    rohit::json::Value *jsonvalue = rohit::json::Value::Parse(json);

    auto newvalue = jsonvalue->GetString();
    return value == newvalue;
}

TEST(JSONTest, StringTest) {
    const std::vector<std::string> values {
        "Test",
        "This is a test",
        "Test escape",
        "Testing String is a good exercise"
    };

    for (auto &value: values)
        EXPECT_TRUE(StringCreateAndMatch(value));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}