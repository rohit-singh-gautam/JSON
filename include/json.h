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
// JSON Libaary based on https://www.rfc-editor.org/rfc/rfc8259

#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>
#include <format>
#include <charconv>

namespace rohit::json {

// Change this to unordered_map in case unordered_map is require.
// unordered_map results int to memory leak error with gcc -fanalyzer
template <typename KeyT, typename ValueT>
using map = std::map<KeyT, ValueT>;

template <typename KeyT>
using vector = std::vector<KeyT>;

struct write_format {
    bool newline_before_braces_open;
    bool newline_after_braces_open;
    bool newline_before_braces_close;
    bool newline_after_braces_close;
    bool newline_before_bracket_open;
    bool newline_after_bracket_open;
    bool newline_before_bracket_close;
    bool newline_after_bracket_close;
    bool newline_before_object_member;
    bool space_after_comma; // Space will not be added if newline is enabled
    bool newline_after_comma;
    bool space_after_colon;
    bool all_data_on_newline;
    std::string intendtext;
};

namespace format {
static constexpr write_format compress { 
    .newline_before_braces_open = false,
    .newline_after_braces_open = false,
    .newline_before_braces_close = false,
    .newline_after_braces_close = false,
    .newline_before_bracket_open = false,
    .newline_after_bracket_open = false,
    .newline_before_bracket_close = false,
    .newline_after_bracket_close = false,
    .newline_before_object_member = false,
    .space_after_comma = false,
    .newline_after_comma = false,
    .space_after_colon = false,
    .all_data_on_newline = false,
    .intendtext = { } 
};

static constexpr write_format beautify { 
    .newline_before_braces_open = false,
    .newline_after_braces_open = true,
    .newline_before_braces_close = true,
    .newline_after_braces_close = false,
    .newline_before_bracket_open = true,
    .newline_after_bracket_open = true,
    .newline_before_bracket_close = true,
    .newline_after_bracket_close = false,
    .newline_before_object_member = true,
    .space_after_comma = true,
    .newline_after_comma = false,
    .space_after_colon = true,
    .all_data_on_newline = false,
    .intendtext = { "  " } 
};

static constexpr write_format beautify_vertical { 
    .newline_before_braces_open = true,
    .newline_after_braces_open = true,
    .newline_before_braces_close = true,
    .newline_after_braces_close = false,
    .newline_before_bracket_open = true,
    .newline_after_bracket_open = true,
    .newline_before_bracket_close = true,
    .newline_after_bracket_close = false,
    .newline_before_object_member = true,
    .space_after_comma = true,
    .newline_after_comma = true,
    .space_after_colon = true,
    .all_data_on_newline = true,
    .intendtext = { "  " } 
};
} // namespace format

struct write_format_data {
    std::string prefix;
    bool newline_added;
    write_format format;
};


#ifdef LIST_DEFINITION_END
    #undef LIST_DEFINITION_END
#endif
#define LIST_DEFINITION_END

#ifdef ERROR_T_LIST
    #undef ERROR_T_LIST
#endif

#define ERROR_T_LIST \
    ERROR_T_ENTRY(SUCCESS, "SUCCESS") \
    ERROR_T_ENTRY(NOT_ARRAY_OR_MAP, "Not an array or map") \
    ERROR_T_ENTRY(NOT_BOOL, "Not a bool") \
    ERROR_T_ENTRY(NOT_NULL, "Not a null") \
    ERROR_T_ENTRY(NOT_INTEGER, "Not an integer") \
    ERROR_T_ENTRY(NOT_FLOAT, "Not a floating point number") \
    ERROR_T_ENTRY(NOT_STRING, "Not a string") \
    ERROR_T_ENTRY(ARRAY_OUT_OF_RANGE, "Array index out of rannge") \
    ERROR_T_ENTRY(OBJECT_OUT_OF_RANGE, "Object index out of rannge") \
    ERROR_T_ENTRY(PREMATURE_JSON_TERMINATION, "Premature JSON termination") \
    ERROR_T_ENTRY(UNKNOWN_KEYWORD, "Unknown keyword") \
    ERROR_T_ENTRY(UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION, "Unknown keyword or premature termination") \
    ERROR_T_ENTRY(INCORRECT_ARRAY_DELIMITER, "Incorrect array delimiter") \
    ERROR_T_ENTRY(BAD_NUMBER_FORMAT, "Bad number format") \
    ERROR_T_ENTRY(BAD_FLOAT_FORMAT, "Bad float format") \
    ERROR_T_ENTRY(INCORRECT_ESCAPE, "Incorrect character in string escape") \
    ERROR_T_ENTRY(INCORRECT_OBJECT_MEMBER_SEPARATOR, "Incorrect separator between Object member, it must be ':'") \
    ERROR_T_ENTRY(OBJECT_NULL_KEY, "Empty key is not allowed in object") \
    ERROR_T_ENTRY(OBJECT_DUPLICATE_KEY, "Duplicate key is not allowed in object") \
    ERROR_T_ENTRY(OBJECT_KEY_STRING_EXPECTED, "Object key is expected to be string") \
    ERROR_T_ENTRY(INCORRECT_OBJECT_DELIMITER, "Incorrect object delimiter") \
    LIST_DEFINITION_END

enum class exception_t {
#define ERROR_T_ENTRY(x, y) x,
        ERROR_T_LIST
#undef ERROR_T_ENTRY
};

constexpr const char *to_string(const exception_t &val) {
    constexpr const char *exception_string[] = {
    #define ERROR_T_ENTRY(x, y) {#x" - " y},
        ERROR_T_LIST
    #undef ERROR_T_ENTRY
    };
    
    auto index = static_cast<size_t>(val);
    return exception_string[index];
}

class Exception : public std::exception {
protected:
    exception_t value;

public:
    Exception(const exception_t value) : value { value } { }
    Exception(Exception &&rhs) : std::exception { std::move(rhs) }, value {rhs.value} { }

    const char* what() const noexcept override { return to_string(value); }
};

class NotArraryOrMapException : public Exception {
public:
    NotArraryOrMapException() : Exception { exception_t::NOT_ARRAY_OR_MAP } { }
};

class NotBoolException : public Exception {
public:
    NotBoolException() : Exception { exception_t::NOT_BOOL } { }
};

class NotNullException : public Exception {
public:
    NotNullException() : Exception { exception_t::NOT_NULL } { }
};

class NotIntegerException : public Exception {
public:
    NotIntegerException() : Exception { exception_t::NOT_INTEGER } { }
};

class NotFloatException : public Exception {
public:
    NotFloatException() : Exception { exception_t::NOT_FLOAT } { }
};

class NotStringException : public Exception {
public:
    NotStringException() : Exception { exception_t::NOT_STRING } { }
};

class ArrayOutOfRangeException : public Exception {
public:
    ArrayOutOfRangeException() : Exception { exception_t::ARRAY_OUT_OF_RANGE } { }
};

class ObjecctOutOfRangeException : public Exception {
public:
    ObjecctOutOfRangeException() : Exception { exception_t::OBJECT_OUT_OF_RANGE } { }
};

class Value;
class JsonParseException : public Exception {
    friend class Value;
    const char *start_position { };
    const char *current_position;
    size_t buffer_remaining;
public:
    JsonParseException(const char *current_position, const size_t buffer_remaining, const exception_t except)
        : Exception { except }, current_position { current_position }, buffer_remaining { buffer_remaining } { }
    JsonParseException(JsonParseException &&rhs) : Exception { std::move(rhs) }, start_position { rhs.start_position }, current_position { rhs.current_position }, buffer_remaining { rhs.buffer_remaining } {
        rhs.start_position = nullptr;
        rhs.current_position = nullptr;
        rhs.buffer_remaining = 0;
    }
    JsonParseException(const JsonParseException &) = delete;
    JsonParseException &operator=(const JsonParseException &) = delete;

    std::string to_string() const {
        std::string errstr { "JSON Parser failed at: " };

        if (current_position < start_position) {
            errstr += "Underflow - ";
        } else 
        {
            if (current_position - start_position >= 40 ){
                for(size_t index { 0 }; index < 16; ++index) {
                    auto current_ch = start_position[index];
                    if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                        errstr.push_back(current_ch);
                    } else errstr.push_back('#');
                }
                errstr += " ... ";

                for(size_t index { 16 }; index; --index) {
                    auto current_ch = *(current_position - index);
                    if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                        errstr.push_back(current_ch);
                    } else errstr.push_back('#');
                }
            } else {
                auto itr = start_position;
                while(itr < current_position) {
                    auto current_ch = *itr;
                    if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                        errstr.push_back(current_ch);
                    } else errstr.push_back(current_ch);

                    ++itr;
                }
            }

            errstr += " <-- failed here ";
            errstr += rohit::json::to_string(value);
            errstr += " --| ";
        }

        for(size_t index { 0 }; index < std::min(16UL, buffer_remaining); ++index) {
            auto current_ch = current_position[index];
            if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                errstr.push_back(current_ch);
            } else errstr.push_back('#');
        }
        if (buffer_remaining > 16) {
            errstr += " ... more ";
            errstr += std::to_string(buffer_remaining - 16UL);
            errstr += " characters.";
        }

        return errstr;
    }

    const char* what() const noexcept override { 
        thread_local static std::string value { };
        value = to_string();
        return value.c_str();
    }

};

class ParseException : public Exception {
public:
    const char* what() const noexcept override { return "Bad Parser this is internal error"; }
};

enum class type {
    Bool, // True and False will be under bool, this will allow us to change it directly
    Null,
    Object,
    Array,
    NumberInt,
    NumberFloat,
    String,
    Error
};

class Value {
public:
    virtual ~Value() = default;

protected:
    static constexpr const char BeginArray { '[' };
    static constexpr const char BeginObject { '{' };
    static constexpr const char EndArray { ']' };
    static constexpr const char EndObject { '}' };
    static constexpr const char NameSeparator { ':' };
    static constexpr const char ValueSeparator { ',' };

    static constexpr auto IsWS(const char ch) {
        /*/
            https://www.rfc-editor.org/rfc/rfc8259 [Page 6]
            ws = *(
                %x20 /              ; Space
                %x09 /              ; Horizontal tab
                %x0A /              ; Line feed or New line
                %x0D )              ; Carriage return
        */
        return ch == 0x20 || ch == 0x09 || ch == 0x0A || ch == 0x0D;
    }

    static constexpr auto SkipWS(const char *&text, size_t &size) {
        while(size && IsWS(*text)) { ++text; --size; }
    }

public:
    virtual constexpr void write(std::string &, write_format_data &) const = 0;

    virtual constexpr type GetType() const noexcept = 0;
    virtual constexpr Value *atptr(size_t) const { throw NotArraryOrMapException { }; }
    virtual constexpr Value *atptr(const std::string &) const { throw NotArraryOrMapException { }; }
    virtual constexpr bool operator==(const Value& other) const = 0;

    virtual constexpr bool &GetBool() { throw NotBoolException { }; }
    virtual constexpr bool GetBool() const { throw NotBoolException { }; }
    virtual constexpr bool IsNull() const { return false; }
    virtual constexpr bool IsError() const { return false; }
    virtual constexpr int &GetInt() { throw NotIntegerException { }; }
    virtual constexpr int GetInt() const { throw NotIntegerException { }; }
    virtual constexpr double &GetFloat() { throw NotFloatException { }; }
    virtual constexpr double GetFloat() const { throw NotFloatException { }; }
    virtual constexpr const std::string_view GetStringView() const { throw NotStringException { }; }
    virtual constexpr std::string &GetString() { throw NotStringException { }; }
    virtual constexpr const std::string &GetString() const { throw NotStringException { }; }
    virtual constexpr const std::string GetStringCopy() const { throw NotStringException { }; }
    virtual constexpr void push_back(const bool) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(const int) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(const double) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(const std::string &) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(std::string &&) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(Value *) { throw NotArraryOrMapException { }; }
    virtual constexpr void push_back(std::unique_ptr<Value> &&) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<int> GetIntVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<bool> GetBoolVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<float> GetFloatVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<std::string> GetStringVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, const bool) { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, const int)  { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, const double)  { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, const std::string &)  { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, std::string &&)  { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, Value *)  { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, std::unique_ptr<Value> &&) { throw NotArraryOrMapException { }; }
    virtual constexpr map<std::string, int> GetIntMap(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr map<std::string, bool> GetBoolMap(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr map<std::string, float> GetFloatMap(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr map<std::string, std::string> GetStringMap(bool) { throw NotArraryOrMapException { }; }

    constexpr Value &operator[](size_t index) const { return *atptr(index); }
    constexpr Value &operator[](const std::string &key) const { return *atptr(key); }

    constexpr auto &at(size_t index) const { return *atptr(index); }
    constexpr auto &at(const std::string &key) const { return *atptr(key); }

    static constexpr Value *Parse(const char *&text, size_t &size) {
        try {
            return ParseInternal(text, size);
        } catch(JsonParseException &jsonexp) {
            jsonexp.start_position = text;
            throw std::move(jsonexp);
        }
    }

    static constexpr Value *Parse(const std::string &text) { 
        const char *textptr = text.c_str();
        size_t size = text.size();
        return Parse(textptr, size);
    }

protected:
    static constexpr Value *ParseInternal(const char *&text, size_t &size);
    static constexpr Value *ParseIntegerOrFloat(const char *&text, size_t &size);

    static constexpr std::string ParseString(const char *&text, size_t &size) {
        if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
        std::string value { };
        while(*text != '"') {
            if (*text == '\\') {
                ++text;
                --size;
                if (size < 2) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
                switch(*text) {
                case '"':
                case '\\':
                    value.push_back(*text);
                    ++text; --size;
                    break;
                case 'b':
                    value.push_back('\b');
                    ++text; --size;
                    break;
                case 'f':
                    value.push_back('\f');
                    ++text; --size;
                    break;
                case 'n':
                    value.push_back('\n');
                    ++text; --size;
                    break;
                case 'r':
                    value.push_back('\r');
                    ++text; --size;
                    break;
                case 't':
                    value.push_back('t');
                    ++text; --size;
                    break;
                case 'u': {
                    ++text; --size;
                    if (size < 6) throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                    char val { 0 };
                    auto newsize = size - 4;
                    do {
                        auto ch = *text;
                        val *= 10;
                        if (ch >= '0' && ch <= '9') val += ch - '0';
                        else if (ch >= 'a' && ch <= 'f') val += 10 + ch - 'a';
                        else if (ch >= 'A' && ch <= 'F') val += 10 + ch - 'A';
                        else throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                        ++text; --size;
                    } while(size != newsize);
                    value.push_back(val);
                    break;
                }
                default:
                    throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                    break;
                }

            } else {
                value.push_back(*text);
                ++text; --size;
                if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
            }
        }
        ++text; --size;
        return value;
    }
};

class Null : public Value {
public:
    constexpr Null() { }

    constexpr bool operator==(const Value& other) const override {
        return other.GetType() == type::Null;
    }
    bool operator==(const Null&) const { return true; };
    bool operator==(const Null&) { return true; };

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        text += "null";
        data.newline_added = false;
    }

    constexpr type GetType() const noexcept override { return type::Null; }
    constexpr bool IsNull() const override { return true; }
    constexpr bool GetBool() const override { return false; }
    constexpr int GetInt() const override { return 0; }
    constexpr double GetFloat() const override { return 0.0; }
    constexpr const std::string GetStringCopy() const override {
        return { "null" };
    }
};

class Error : public Value {
    constexpr Error() { }
public:
    constexpr Error(const Error &) { }
    static Error error;

    constexpr type GetType() const noexcept override { return type::Error; }
    constexpr bool IsError() const override { return true; }
    constexpr bool operator==(const Value& other) const override {
        return other.GetType() == type::Error;
    }
    bool operator==(const Error&) const { return true; };
    bool operator==(const Error&) { return true; };

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        text += "error";
        data.newline_added = false;
    }
};

inline Error Error::error { };

class Ref {
protected:
    std::unique_ptr<Value> obj;

public:
    constexpr Ref(const std::string &text) : obj { Value::Parse(text) } { }
    constexpr Ref(const char *&text, size_t &size) : obj { Value::Parse(text, size) } { }
    constexpr Ref(Value *obj) : obj { obj } { }
    constexpr Ref();
    Ref(const Ref &) = delete;

    Ref &operator=(const Ref &) = delete;

    constexpr inline type GetType() const { return obj->GetType(); }

    constexpr inline Value *atptr(size_t index) const { return obj->atptr(index); }
    constexpr inline Value *atptr(const std::string &key) const { return obj->atptr(key); }
    constexpr inline Value &at(size_t index) const { return obj->at(index); }
    constexpr inline Value &at(const std::string &key) const { return obj->at(key); }
    constexpr inline Value &operator[](size_t index) const { return obj->operator[](index); }
    constexpr inline Value &operator[](const std::string &key) const { return obj->operator[](key); }
    constexpr inline bool operator==(const Value& other) const { return obj->operator==(other); }
    constexpr inline bool operator==(const Ref& other) const { return obj->operator==(*other.obj); }

    constexpr inline bool &GetBool() { return obj->GetBool(); }
    constexpr inline bool GetBool() const { return obj->GetBool(); }
    constexpr inline bool IsNull() const { return obj->IsNull(); }
    constexpr inline int &GetInt() { return obj->GetInt(); }
    constexpr inline int GetInt() const { return obj->GetInt(); }
    constexpr inline double &GetFloat() { return obj->GetFloat(); }
    constexpr inline double GetFloat() const { return obj->GetFloat(); }
    constexpr inline const std::string_view GetStringView() const { return obj->GetStringView(); }
    constexpr inline std::string &GetString() { return obj->GetString(); }
    constexpr inline const std::string &GetString() const { return obj->GetString(); }
    constexpr inline const std::string GetStringCopy() const { return obj->GetStringCopy(); }
    constexpr inline void push_back(const bool value) { return obj->push_back(value); }
    constexpr inline void push_back(const int value) { return obj->push_back(value); }
    constexpr inline void push_back(const double value) { return obj->push_back(value); }
    constexpr inline void push_back(const std::string &value) { return obj->push_back(value); }
    constexpr inline void push_back(std::string &&value) { return obj->push_back(std::move(value)); }
    constexpr inline void push_back(Value *value) { return obj->push_back(value); }
    constexpr inline void push_back(std::unique_ptr<Value> &&value) { return obj->push_back(std::move(value)); }
    constexpr inline vector<int> GetIntVector(bool ignore_exceptions) { return obj->GetIntVector(ignore_exceptions); }
    constexpr inline vector<bool> GetBoolVector(bool ignore_exceptions) { return obj->GetBoolVector(ignore_exceptions); }
    constexpr inline vector<float> GetFloatVector(bool ignore_exceptions) { return obj->GetFloatVector(ignore_exceptions); }
    constexpr inline vector<std::string> GetStringVector(bool ignore_exceptions) { return obj->GetStringVector(ignore_exceptions); }
    constexpr inline void insert(std::string key, const bool value) { return obj->insert(key, value); }
    constexpr inline void insert(std::string key, const int value) { return obj->insert(key, value); }
    constexpr inline void insert(std::string key, const double value) { return obj->insert(key, value); }
    constexpr inline void insert(std::string key, const std::string &value) { return obj->insert(key, value); }
    constexpr inline void insert(std::string key, std::string &&value) { return obj->insert(key, std::move(value)); }
    constexpr inline void insert(std::string key, Value *value) { return obj->insert(key, value); }
    constexpr inline void insert(std::string key, std::unique_ptr<Value> &&value) { return obj->insert(key, std::move(value)); }
    constexpr inline map<std::string, int> GetIntMap(bool ignore_exceptions) { return obj->GetIntMap(ignore_exceptions); }
    constexpr inline map<std::string, bool> GetBoolMap(bool ignore_exceptions) { return obj->GetBoolMap(ignore_exceptions); }
    constexpr inline map<std::string, float> GetFloatMap(bool ignore_exceptions) { return obj->GetFloatMap(ignore_exceptions); }
    constexpr inline map<std::string, std::string> GetStringMap(bool ignore_exceptions) { return obj->GetStringMap(ignore_exceptions); }

    constexpr std::string write(const write_format &format) {
        std::string result { };
        write_format_data dataformat {{}, true, format };
        obj->write(result, dataformat);
        return result;
    }

    /// @brief 
    /// @param text 
    /// @param delimiter 
    /// @return 
    constexpr Value &Query(const std::string &text, const auto &delimiter);

    constexpr Value &Query(const std::string &text) {
        return Query(text, '/');
    }
};

class Bool : public Value {
protected:
    bool value;

public:
    constexpr Bool(const Bool &other) : value { other.value } { }
    constexpr Bool(bool value) : value { value } { }

    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Bool *>(&other);
        return value == rhs->value;
    }
    bool operator==(const Bool& other) const = delete;
    bool operator==(const Bool& other) = delete;

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        if (value) text += "true";
        else text += "false";
        data.newline_added = false;
    }

    constexpr type GetType() const noexcept override { return type::Bool; }

    constexpr bool &GetBool() override { return value; }
    constexpr bool GetBool() const override { return value; }
    constexpr int GetInt() const override { return static_cast<int>(value); }
    constexpr double GetFloat() const override { return static_cast<double>(value); }
    constexpr const std::string GetStringCopy() const override {
        if (value) return { "true" };
        else return { "false" };
    }
};

class Integer : public Value {
    int value;
    
public:
    constexpr Integer(const Integer &other) : value { other.value } { }
    constexpr Integer(const int value) : value { value } { }

    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Integer *>(&other);
        return value == rhs->value;
    }
    bool operator==(const Integer& other) const = delete;
    bool operator==(const Integer& other) = delete;

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        text += std::to_string(value);
        data.newline_added = false;
    }

    constexpr type GetType() const noexcept override { return type::NumberInt; }

    constexpr bool GetBool() const override { return value != 0; }
    constexpr int &GetInt() override { return value; }
    constexpr int GetInt() const override { return value; }
    constexpr double GetFloat() const override { return static_cast<double>(value); }
    constexpr const std::string GetStringCopy() const override { return std::to_string(value); }
};

class Float : public Value {
    double value;

public:
    constexpr Float(const Float &other) : value { other.value } { }
    constexpr Float(const double value) : value { value } { }

    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Float *>(&other);
        return value == rhs->value;
    }
    bool operator==(const Float& other) const = delete;
    bool operator==(const Float& other) = delete;

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        text += std::to_string(value);
        data.newline_added = false;
    }

    constexpr type GetType() const noexcept override { return type::NumberFloat; }

    constexpr bool GetBool() const override { return value != 0; }
    constexpr int GetInt() const override { return static_cast<int>(value); }
    constexpr double GetFloat() const override { return value; }
    constexpr double &GetFloat() override { return value; }
    constexpr const std::string GetStringCopy() const override { return std::to_string(value); }
};

class String : public Value {
    std::string value;

public:
    constexpr String(const String &other) : value { other.value } { }
    constexpr String(const std::string &value) : value { value } { }
    constexpr String(std::string &&value) : value { std::move(value) } { }

    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const String *>(&other);
        return value == rhs->value;
    }
    constexpr bool operator==(const String& other) const = delete;
    constexpr bool operator==(const String& other) = delete;

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.all_data_on_newline && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        text.push_back('"');
        EscapeString(value, text);
        text.push_back('"');
        data.newline_added = false;
    }

    constexpr const std::string_view GetStringView() const override { return { value.c_str(), value.size() }; }

    constexpr bool GetBool() const override { 
        if (value == "true") return true;
        if (value == "false") return true;
        int ival = std::stoi(value);
        return !!ival;
    }
    constexpr int GetInt() const override { return std::stoi(value); }
    constexpr double GetFloat() const override { return std::stod(value); }
    constexpr std::string &GetString() override { return value; }
    constexpr const std::string &GetString() const override { return value; }
    constexpr const std::string GetStringCopy() const override { return value; }

    constexpr type GetType() const noexcept override { return type::String; }

    static constexpr Value *Parse(const char *&text, size_t &size) {
        auto value = ParseString(text, size);
        return new String { std::move(value) };
    }

    // This may not be exact original value
    static constexpr void EscapeString(const std::string &original, std::string &newstr) {
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
                if (*itr >= 32) newstr.push_back(*itr);
                else {
                    const auto val = static_cast<uint8_t>(*itr);
                    newstr += "u00";
                    newstr.push_back('0' + (val/16));
                    newstr.push_back('0' + (val%16));
                }
                break;
            }
            itr = std::next(itr);
        }
    }
};


class Array : public Value {
    vector<std::unique_ptr<Value>> values { };

public:
    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Array *>(&other);
        if (values.size() != rhs->values.size()) return false;
        for(size_t index { 0 }; index < values.size(); ++index) {
            if (*values[index] != *rhs->values[index]) return false;
        }
        return true;
    }
    bool operator==(const Array& other) const = delete;
    bool operator==(const Array& other) = delete;

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.newline_before_bracket_open && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        if (values.empty()) text += "[]";
        else {
            text.push_back('[');
            data.prefix.append(data.format.intendtext);
            if (data.format.newline_after_bracket_open) {
                text += '\n';
                text += data.prefix;
                data.newline_added = true;
            }
            auto itr = std::begin(values);
            (*itr)->write(text, data);
            itr = std::next(itr);
            while(itr != std::end(values)) {
                text.push_back(',');
                if (data.format.newline_after_comma) {
                    text += '\n';
                    text += data.prefix;
                    data.newline_added = true;
                } else if (data.format.space_after_comma) text.push_back(' ');
                (*itr)->write(text, data);
                itr = std::next(itr);
            }
            const auto prefixsize = data.prefix.size();
            const auto intendsize = data.format.intendtext.size();
            data.prefix.erase(prefixsize - intendsize, intendsize);
            if (data.format.newline_before_bracket_close) {
                text += '\n';
                text += data.prefix;
            }
            text.push_back(']');
        }
        if (data.format.newline_after_bracket_close) {
            text += '\n';
            text += data.prefix;
            data.newline_added = true;
        } else data.newline_added = false;
    }

    constexpr Value *atptr(size_t index) const override { 
        if (index >= values.size()) return &Error::error;
        return values[index].get();
    }
    constexpr Value *atptr(const std::string &key) const override {
        auto index = std::stoul(key);
        if (index >= values.size()) return &Error::error;;
        return values[index].get();
    }

    constexpr void push_back(const bool value) override {
        auto json = new Bool { value };
        values.emplace_back(json);
    }

    constexpr void push_back(const int value) override {
        auto json = new Integer { value };
        values.emplace_back(json);
    }

    constexpr void push_back(const double value) override {
        auto json = new Float { value };
        values.emplace_back(json);
    }

    constexpr void push_back(const std::string &value) override {
        auto json = new String { value };
        values.emplace_back(json);
    }

    constexpr void push_back(std::string &&value) override {
        auto json = new String { std::move(value) };
        values.emplace_back(json);
    }

    constexpr void push_back(Value *json) override {
        values.emplace_back(json);
    }

    constexpr void push_back(std::unique_ptr<Value> &&json) override {
        values.push_back(std::move(json));
    }

    /*! This will return all the integer, it will try to convert it to integer for string and bool.
     * if any of fails to retrieve it will throw exception if exclude_non_int is set otherwise it will thorw exception
     */
    constexpr vector<int> GetIntVector(bool exclude_non_int = true) override {
        vector<int> ret { };
        if (exclude_non_int) {
            for(auto &value: values) {
                try {
                    ret.push_back(value->GetInt());
                } catch(NotIntegerException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value->GetInt());
            }
        }
        return ret;
    }

    constexpr vector<bool> GetBoolVector(bool exclude_non_bool = true) override {
        vector<bool> ret { };
        if (exclude_non_bool) {
            for(auto &value: values) {
                try {
                    ret.push_back(value->GetBool());
                } catch(NotBoolException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value->GetBool());
            }
        }
        return ret;
    }

    constexpr vector<float> GetFloatVector(bool exclude_non_float = true) override {
        vector<float> ret { };
        if (exclude_non_float) {
            for(auto &value: values) {
                try {
                    ret.push_back(value->GetFloat());
                } catch(NotFloatException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value->GetFloat());
            }
        }
        return ret;
    }

    constexpr vector<std::string> GetStringVector(bool exclude_non_string = true) override {
        vector<std::string> ret { };
        if (exclude_non_string) {
            for(auto &value: values) {
                try {
                    ret.push_back(std::move(value->GetStringCopy()));
                } catch(NotStringException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(std::move(value->GetStringCopy()));
            }
        }
        return ret;
    }

    constexpr type GetType() const noexcept override { return type::Array; }


    static constexpr Value *Parse(const char *&text, size_t &size) {
        if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
        Array *array = new Array { };
        SkipWS(text, size);
        if (*text != ']') {
            for(;;) {
                auto value = Value::ParseInternal(text, size);
                array->values.emplace_back(value);
                if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
                if (*text == ']') break;
                if (*text != ',') {
                    throw JsonParseException { text, size, exception_t::INCORRECT_ARRAY_DELIMITER };
                }
                ++text; --size;
                SkipWS(text, size);
            }
        }
        ++text; --size;
        return array;
    }
};

class Object : public Value {
    // unordered_map fails with -fanalyzer
    map<std::string, std::unique_ptr<Value>> values { };
    
public:
    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Object *>(&other);
        if (values.size() != rhs->values.size()) return false;
        for(auto &value: values) {
            auto itr = rhs->values.find(value.first);
            if (itr == std::end(rhs->values)) return false;
            if (*value.second != *itr->second) return false;
        }
        return true;
    }
    bool operator==(const Object& other) const = delete;
    bool operator==(const Object& other) = delete;
    

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.newline_before_bracket_open && !data.newline_added) {
            text += '\n';
            text += data.prefix;
        }
        if (values.empty()) text += "{}";
        else {
            text.push_back('{');
            data.prefix.append(data.format.intendtext);
            if (data.format.newline_after_braces_open || data.format.newline_before_object_member) {
                text += '\n';
                text += data.prefix;
                data.newline_added = true;
            }
            auto itr = std::begin(values);
            text.push_back('"');
            text += itr->first;
            text.push_back('"');
            text.push_back(':');
            if (itr->second->GetType() == type::Object || itr->second->GetType() == type::Array)
                data.newline_added = false;
            else if (data.format.space_after_colon) text.push_back(' ');
            itr->second->write(text, data);
            itr = std::next(itr);
            while(itr != std::end(values)) {
                text.push_back(',');
                if (data.format.newline_after_comma || data.format.newline_before_object_member) {
                    text += '\n';
                    text += data.prefix;
                    data.newline_added = true;
                } else if (data.format.space_after_comma) text.push_back(' ');
                text.push_back('"');
                text += itr->first;
                text.push_back('"');
                text.push_back(':');
                if (itr->second->GetType() == type::Object || itr->second->GetType() == type::Array)
                    data.newline_added = false;
                else if (data.format.space_after_colon) text.push_back(' ');
                itr->second->write(text, data);
                itr = std::next(itr);
            }
            const auto prefixsize = data.prefix.size();
            const auto intendsize = data.format.intendtext.size();
            data.prefix.erase(prefixsize - intendsize, intendsize);
            if (data.format.newline_before_braces_close) {
                text += '\n';
                text += data.prefix;
            }
            text.push_back('}');
        }
        if (data.format.newline_after_braces_close) {
            text += '\n';
            text += data.prefix;
            data.newline_added = true;
        } else data.newline_added = false;
    }

    Value *atptr(size_t index) const override { 
        auto key = std::to_string(index);
        auto itr = values.find(key);
        if (itr == std::end(values)) return &Error::error;
        return itr->second.get();
    }
    Value *atptr(const std::string &key) const override {
        auto itr = values.find(key);
        if (itr == std::end(values)) return &Error::error;
        return itr->second.get();
    }

    constexpr type GetType() const noexcept override { return type::Object; }

    static constexpr void ParseMember(Object *obj, const char *&text, size_t &size) {
        auto key = ParseString(text, size);
        SkipWS(text, size);
        if (*text != ':') throw JsonParseException(text, size, exception_t::INCORRECT_OBJECT_MEMBER_SEPARATOR);
        ++text; --size;
        auto value = Value::ParseInternal(text, size);
        if (key.empty()) throw JsonParseException(text, size, exception_t::OBJECT_NULL_KEY);
        auto itr = obj->values.find(key);
        if (itr != std::end(obj->values)) throw JsonParseException(text, size, exception_t::OBJECT_DUPLICATE_KEY);
        obj->values.emplace(key, value);
    }

    static constexpr Value *Parse(const char *&text, size_t &size) {
        SkipWS(text, size);
        auto value = new Object { };
        if (*text != '}') {
            for(;;) {
                if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
                if (*text != '"') throw JsonParseException(text, size, exception_t::OBJECT_KEY_STRING_EXPECTED);
                ++text; --size;
                ParseMember(value, text, size);
                if (*text == '}') break;
                if (*text != ',') throw JsonParseException(text, size, exception_t::INCORRECT_OBJECT_DELIMITER);
                ++text; --size;
                SkipWS(text, size);
                // Allowing commma at the end
                if (*text == '}') break;
            }
        }
        ++text; --size;
        return value;
    }

    constexpr void insert(std::string key, const bool value) override {
        auto json = new Bool { value };
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, const int value) override {
        auto json = new Integer { value };
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, const double value) override {
        auto json = new Float { value };
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, const std::string &value) override {
        auto json = new String { value };
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, std::string &&value) override {
        auto json = new String { std::move(value) };
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, Value *json) override {
        values.emplace(key, json);
    }

    constexpr void insert(std::string key, std::unique_ptr<Value> &&json) override {
        values.emplace(key, std::move(json));
    }

    constexpr vector<int> GetIntVector(bool exclude_non_int = true) override {
        vector<int> ret { };
        if (exclude_non_int) {
            for(auto &value: values) {
                try {
                    ret.push_back(value.second->GetInt());
                } catch(NotIntegerException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value.second->GetInt());
            }
        }
        return ret;
    }

    constexpr vector<bool> GetBoolVector(bool exclude_non_bool = true) override {
        vector<bool> ret { };
        if (exclude_non_bool) {
            for(auto &value: values) {
                try {
                    ret.push_back(value.second->GetBool());
                } catch(NotBoolException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value.second->GetBool());
            }
        }
        return ret;
    }

    constexpr vector<float> GetFloatVector(bool exclude_non_float = true) override {
        vector<float> ret { };
        if (exclude_non_float) {
            for(auto &value: values) {
                try {
                    ret.push_back(value.second->GetFloat());
                } catch(NotFloatException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value.second->GetFloat());
            }
        }
        return ret;
    }

    constexpr vector<std::string> GetStringVector(bool exclude_non_string = true) override {
        vector<std::string> ret { };
        if (exclude_non_string) {
            for(auto &value: values) {
                try {
                    ret.push_back(value.second->GetStringCopy());
                } catch(NotStringException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.push_back(value.second->GetStringCopy());
            }
        }
        return ret;
    }

    constexpr map<std::string, int> GetIntMap(bool exclude_non_int = true) override {
        map<std::string, int> ret { };
        if (exclude_non_int) {
            for(auto &value: values) {
                try {
                    ret.emplace(value.first, value.second->GetInt());
                } catch(NotIntegerException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.emplace(value.first, value.second->GetInt());
            }
        }
        return ret;
    }

    constexpr map<std::string, bool> GetBoolMap(bool exclude_non_bool = true) override {
        map<std::string, bool> ret { };
        if (exclude_non_bool) {
            for(auto &value: values) {
                try {
                    ret.emplace(value.first, value.second->GetBool());
                } catch(NotBoolException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.emplace(value.first, value.second->GetBool());
            }
        }
        return ret;
    }

    constexpr map<std::string, float> GetFloatMap(bool exclude_non_float = true) override {
        map<std::string, float> ret { };
        if (exclude_non_float) {
            for(auto &value: values) {
                try {
                    ret.emplace(value.first, value.second->GetFloat());
                } catch(NotFloatException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.emplace(value.first, value.second->GetFloat());
            }
        }
        return ret;
    }

    constexpr map<std::string, std::string> GetStringMap(bool exclude_non_string = true) override {
        map<std::string, std::string> ret { };
        if (exclude_non_string) {
            for(auto &value: values) {
                try {
                    ret.emplace(value.first, value.second->GetString());
                } catch(NotStringException &) {}
            }
        } else {
            for(auto &value: values) {
                ret.emplace(value.first, value.second->GetString());
            }
        }
        return ret;
    }
}; // class Object

constexpr Value *Value::ParseIntegerOrFloat(const char *&text, size_t &size) {
    if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
    auto itr = text;
    auto end = text + size;
    if (*itr == '-' || *itr == '+') ++itr;
    if (itr == end) throw JsonParseException { itr, size, exception_t::BAD_NUMBER_FORMAT };
    if (*itr < '0' || *itr > '9') throw JsonParseException { itr, size, exception_t::BAD_NUMBER_FORMAT };
    ++itr;
    // Find text, float, string or error
    bool is_float = false;

    while(itr !=end) {
        if (*itr == '.') {
            is_float = true;
            break;
        } else if (*itr < '0' || *itr > '9') break;
        ++itr;
    };

    if (is_float) {
        double value { };
        auto ret = std::from_chars(text, end, value);
        size -= ret.ptr - text;
        text = ret.ptr;
        return new Float { value };
    } else {
        int value { };
        auto ret = std::from_chars(text, end, value);
        size -= ret.ptr - text;
        text = ret.ptr;
        return new Integer { value };
    }
}

// Constexpr makes it inline and avoid duplicate definitions warning
constexpr Value *Value::ParseInternal(const char *&text, size_t &size) {
    Value::SkipWS(text, size);
    switch(*text) {
    case BeginArray: {
            ++text; --size;
            auto ret = Array::Parse(text, size);
            Value::SkipWS(text, size);
            return ret;
        }

    case BeginObject: {
            ++text; --size;
            auto ret = Object::Parse(text, size);
            Value::SkipWS(text, size);
            return ret;
        }

    case '-': {
            auto ret = Value::ParseIntegerOrFloat(text, size);
            Value::SkipWS(text, size);
            return ret;
        }
    case '+': {
            auto ret = Value::ParseIntegerOrFloat(text, size);
            Value::SkipWS(text, size);
            return ret;
        }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
            auto ret = Value::ParseIntegerOrFloat(text, size);
            Value::SkipWS(text, size);
            return ret;
        }
    case '"': {
            ++text; --size;
            auto ret = String::Parse(text, size);
            Value::SkipWS(text, size);
            return ret;
        }
    case 'f':    
    case 'F': {
            if (size < 5) throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
            ++text;
            if (*text != 'a' && *text != 'A') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'l' && *text != 'L') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 's' && *text != 'S') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'e' && *text != 'E') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            size -= 5;
            Value::SkipWS(text, size);
            return new Bool { false };
        }
    case 't':
    case 'T': {
            if (size < 4) throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
            ++text;
            if (*text != 'r' && *text != 'R') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'u' && *text != 'U') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'e' && *text != 'E') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            size -= 4;
            Value::SkipWS(text, size);
            return new Bool { true };
        }
    case 'n':    
    case 'N': {
            if (size < 4) throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
            ++text;
            if (*text != 'u' && *text != 'U') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'l' && *text != 'L') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            if (*text != 'l' && *text != 'L') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            ++text;
            size -= 4;
            Value::SkipWS(text, size);
            return new Null { };
        }
    default:
        throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
    }
}

template <typename DT>
constexpr inline size_t GetLength(const DT &val) {
    return sizeof(val);
}

template <>
constexpr inline size_t GetLength<std::string>(const std::string &val) {
    return val.size();
}

template <>
constexpr inline size_t GetLength<std::string_view>(const std::string_view &val) {
    return val.size();
}


constexpr Value &Ref::Query(const std::string &text, const auto &delimiter) {
    if (text.empty()) return *obj;
    size_t first = 0;
    size_t last = text.find(delimiter);
    Value *curr = obj.get();
    if (curr == nullptr) return Error::error;
    while(last != std::string::npos) {
        if (first != last) {
            if (curr->GetType() != type::Array && curr->GetType() != type::Object) return Error::error;
            auto key = text.substr(first, last - first);
            curr = curr->atptr(key);
            if (curr == nullptr) return Error::error;
        }
        first = last + GetLength(delimiter);
        last = text.find(delimiter, first);
    }

    if (first != text.size()) {
        if (curr->GetType() != type::Array && curr->GetType() != type::Object) return Error::error;
        auto key = text.substr(first);
        curr = curr->atptr(key);
    }

    if (curr == nullptr) return Error::error;
    
    return *curr;
}

constexpr Ref::Ref() : obj { new Object { } } { }

constexpr inline auto Parse(const char *&text, size_t &size) { return Ref { text, size }; }
constexpr inline auto Parse(const std::string &text) { return Ref { text }; }

} // namespace rohit::json