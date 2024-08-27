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
#include <set>
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
    ERROR_T_ENTRY(STREAM_UNDERFLOW, "Stream moved before begin") \
    ERROR_T_ENTRY(STREAM_OVERFLOW, "Stream moved to end") \
    ERROR_T_ENTRY(NOT_ARRAY_OR_MAP, "Not an array or map") \
    ERROR_T_ENTRY(NOT_BOOL, "Not a bool") \
    ERROR_T_ENTRY(NOT_NULL, "Not a null") \
    ERROR_T_ENTRY(NOT_INTEGER, "Not an integer") \
    ERROR_T_ENTRY(NOT_FLOAT, "Not a floating point number") \
    ERROR_T_ENTRY(NOT_STRING, "Not a string") \
    ERROR_T_ENTRY(NOT_MEMBER, "Not an object member") \
    ERROR_T_ENTRY(NOT_OBJECT, "Not an object") \
    ERROR_T_ENTRY(NOT_ARRAY, "Not an array") \
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

class StreamException : public Exception {
public:
    using Exception::Exception;
};

class StreamUnderflowException : public StreamException {
public:
    StreamUnderflowException() : StreamException { exception_t::STREAM_UNDERFLOW } { }
};

class StreamOverflowException : public StreamException {
public:
    StreamOverflowException() : StreamException { exception_t::STREAM_OVERFLOW } { }
};

class NotArraryOrMapException : public Exception {
public:
    NotArraryOrMapException() : Exception { exception_t::NOT_ARRAY_OR_MAP } { }
};

class NotObjectException : public Exception {
public:
    NotObjectException() : Exception { exception_t::NOT_OBJECT } { }
};

class NotMemberException : public Exception {
public:
    NotMemberException() : Exception { exception_t::NOT_MEMBER } { }
};

class NotArrayException : public Exception {
public:
    NotArrayException() : Exception { exception_t::NOT_ARRAY } { }
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

class ObjectOutOfRangeException : public Exception {
public:
    ObjectOutOfRangeException() : Exception { exception_t::OBJECT_OUT_OF_RANGE } { }
};

class Stream {
    const char * _begin;
    const char * _end;
    const char *_curr;

public:
    constexpr Stream(const char *_begin, const char *_end) : _begin { _begin }, _end { _end }, _curr { _begin } { }
    constexpr Stream(const char *_begin, size_t size) : _begin { _begin }, _end { _begin + size }, _curr { _begin } { }
    constexpr Stream(const std::string &text) : _begin { text.c_str() }, _end { text.c_str() + text.size() }, _curr { _begin } { }
    constexpr Stream(Stream &&stream) : _begin { stream._begin }, _end { stream._end }, _curr { stream._curr } {
        _begin = _end = _curr = nullptr;
    }
    Stream(const Stream &stream) : _begin { stream._begin }, _end { stream._end }, _curr { stream._curr } { }
    Stream &operator=(const Stream &stream) { _curr = stream._curr; return *this; }

    constexpr auto operator*() const { 
        if (_curr >= _end) throw StreamOverflowException { };
        return *_curr;
    }
    constexpr Stream &operator++() {
        ++_curr;
        return *this;
    }
    constexpr Stream &operator--() { 
        if (_curr == _begin) throw StreamUnderflowException { };
        --_curr;
        return *this;
    }
    Stream operator++(int) = delete;
    Stream operator--(int) = delete;
    constexpr auto operator[](size_t index) const { return _begin[index]; }

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

    void SkipWS() {
        while(_curr != _end && IsWS(*_curr)) { ++_curr; }
    }

    auto begin() const { return _begin; }
    auto end() const { return _end; }
    auto curr() const { return _curr; }

    auto size() const { return static_cast<size_t>(_curr - _begin); }
    auto remaining_size() const { return static_cast<size_t>(_end - _curr); }
    auto capacity() const { return static_cast<size_t>(_end - _begin); }

    void UpdateCurr(const char *_curr) { this->_curr = _curr; }

    template <bool casesensitive = false, const size_t _size>
    constexpr void Match(const char (&text)[_size]);
};

class Value;
class JsonParseException : public Exception {
    const Stream stream;
public:
    JsonParseException(const Stream &stream, const exception_t except)
        : Exception { except }, stream { stream } { }
    JsonParseException(JsonParseException &&rhs) : Exception { std::move(rhs) }, stream { std::move(rhs.stream) } {}
    JsonParseException(const JsonParseException &) = delete;
    JsonParseException &operator=(const JsonParseException &) = delete;

    std::string to_string() const {
        std::string errstr { "JSON Parser failed at: " };

        if (stream.size() >= 40 ){
            std::string_view initial {stream.begin(), 16};
            for(auto &current_ch: initial) {
                if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                    errstr.push_back(current_ch);
                } else errstr.push_back('#');
            }
            errstr += " ... ";

            std::string_view second {stream.curr() - 16, 16};
            for(auto &current_ch: initial) {
                if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                    errstr.push_back(current_ch);
                } else errstr.push_back('#');
            }
        } else {
            std::string_view initial {stream.begin(), stream.size()};
            for(auto &current_ch: initial) {
                if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                    errstr.push_back(current_ch);
                } else errstr.push_back('#');
            }
        }

        errstr += " <-- failed here ";
        errstr += rohit::json::to_string(value);
        errstr += " --| ";

        std::string_view last {stream.curr(), std::min<size_t>(16, stream.remaining_size())};
        for(auto &current_ch: last) {
            if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                errstr.push_back(current_ch);
            } else errstr.push_back('#');
        }
        if (stream.remaining_size() > 16) {
            errstr += " ... more ";
            errstr += std::to_string(stream.remaining_size() - 16UL);
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

template <bool casesensitive, const size_t _size>
constexpr void Stream::Match(const char (&text)[_size]) {
    // Adding + 1 to index to skip null termination
    if (remaining_size() + 1 < _size) throw JsonParseException { *this, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
    for(size_t index { 0 }; index + 1 < _size; ++index) {
        if constexpr (casesensitive)  {
            if (*_curr != text[index]) throw exception_t::UNKNOWN_KEYWORD;
        } else {
            if (*_curr != std::tolower(text[index])) throw exception_t::UNKNOWN_KEYWORD;
        }
        ++_curr;
    }
}

class ParseException : public Exception {
public:
    const char* what() const noexcept override { return "Bad Parser this is internal error"; }
};

enum class type {
    Bool, // True and False will be under bool, this will allow us to change it directly
    Null,
    NumberInt,
    NumberFloat,
    String,
    Member,
    Array,
    Object,
    Error
};

class Iterator_Internal {
public:
    virtual ~Iterator_Internal() = default;
    virtual bool Equal(const Iterator_Internal *other) = 0;
    virtual bool NotEqual(const Iterator_Internal *other) = 0;
    virtual Value &operator*() = 0;
    virtual Value *operator->() = 0;
    virtual void Increment() = 0;
};

class Iterator {
    Iterator_Internal *itr;
public:
    Iterator(Iterator_Internal *itr) : itr { itr } { }
    Iterator(const Iterator &) = delete;
    ~Iterator() { delete itr; }
    Iterator &operator=(const Iterator &) = delete;
    bool operator==(const Iterator &other) { return itr->Equal(other.itr); }
    bool operator!=(const Iterator &other) { return itr->NotEqual(other.itr); }
    Value &operator*() { return itr->operator*(); }
    Value *operator->() { return itr->operator->(); }
    Iterator &operator++() { itr->Increment(); return *this;}
};

class Iterator_const {
    Iterator_Internal *itr;
public:
    Iterator_const(Iterator_Internal *itr) : itr { itr } { }
    Iterator_const(const Iterator_const &) = delete;
    ~Iterator_const() { delete itr; }
    Iterator_const &operator=(const Iterator_const &) = delete;
    bool operator==(const Iterator_const &other) { return itr->Equal(other.itr); }
    bool operator!=(const Iterator_const &other) { return itr->NotEqual(other.itr); }
    const Value &operator*() { return itr->operator*(); }
    const Value *operator->() { return itr->operator->(); }
    Iterator_const &operator++() { itr->Increment(); return *this;}
};

class Reverse_Iterator {
    Iterator_Internal *itr;
public:
    Reverse_Iterator(Iterator_Internal *itr) : itr { itr } { }
    Reverse_Iterator(const Reverse_Iterator &) = delete;
    ~Reverse_Iterator() { delete itr; }
    Reverse_Iterator &operator=(const Reverse_Iterator &) = delete;
    bool operator==(const Reverse_Iterator &other) { return itr->Equal(other.itr); }
    bool operator!=(const Reverse_Iterator &other) { return itr->NotEqual(other.itr); }
    Value &operator*() { return itr->operator*(); }
    Value *operator->() { return itr->operator->(); }
    Reverse_Iterator &operator++() { itr->Increment(); return *this;}
};

class Reverse_Iterator_const {
    Iterator_Internal *itr;
public:
    Reverse_Iterator_const(Iterator_Internal *itr) : itr { itr } { }
    Reverse_Iterator_const(const Reverse_Iterator_const &) = delete;
    ~Reverse_Iterator_const() { delete itr; }
    Reverse_Iterator_const &operator=(const Reverse_Iterator_const &) = delete;
    bool operator==(const Reverse_Iterator_const &other) { return itr->Equal(other.itr); }
    bool operator!=(const Reverse_Iterator_const &other) { return itr->NotEqual(other.itr); }
    const Value &operator*() { return itr->operator*(); }
    const Value *operator->() { return itr->operator->(); }
    Reverse_Iterator_const &operator++() { itr->Increment(); return *this;}
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

public:
    virtual constexpr void write(std::string &, write_format_data &) const = 0;

    virtual constexpr type GetType() const noexcept = 0;
    constexpr bool IsNull() const { return GetType() == type::Null; }
    constexpr bool IsError() const { return GetType() == type::Error; }
    constexpr bool IsBool() const { return GetType() == type::Bool; }
    constexpr bool IsInteger() const { return GetType() == type::NumberInt; }
    constexpr bool IsFloat() const { return GetType() == type::NumberFloat; }
    constexpr bool IsString() const { return GetType() == type::String; }
    constexpr bool IsMember() const { return GetType() == type::Member; }
    constexpr bool IsArray() const { return GetType() == type::Array; }
    constexpr bool IsObject() const { return GetType() == type::Object; }

    virtual constexpr Value *atptr(size_t) const { throw NotArraryOrMapException { }; }
    virtual constexpr Value *atptr(const std::string &) const { throw NotArraryOrMapException { }; }
    virtual constexpr bool operator==(const Value& other) const = 0;

    virtual constexpr bool &GetBool() { throw NotBoolException { }; }
    virtual constexpr bool GetBool() const { throw NotBoolException { }; }
    virtual constexpr int &GetInt() { throw NotIntegerException { }; }
    virtual constexpr int GetInt() const { throw NotIntegerException { }; }
    virtual constexpr double &GetFloat() { throw NotFloatException { }; }
    virtual constexpr double GetFloat() const { throw NotFloatException { }; }
    virtual constexpr const std::string_view GetStringView() const { throw NotStringException { }; }
    virtual constexpr std::string &GetString() { throw NotStringException { }; }
    virtual constexpr const std::string &GetString() const { throw NotStringException { }; }
    virtual constexpr const std::string GetStringCopy() const { throw NotStringException { }; }
    virtual constexpr void push_back(const bool) { throw NotArrayException { }; }
    virtual constexpr void push_back(const int) { throw NotArrayException { }; }
    virtual constexpr void push_back(const double) { throw NotArrayException { }; }
    virtual constexpr void push_back(const std::string &) { throw NotArrayException { }; }
    virtual constexpr void push_back(std::string &&) { throw NotArrayException { }; }
    virtual constexpr void push_back(Value *) { throw NotArrayException { }; }
    virtual constexpr void push_back(std::unique_ptr<Value> &&) { throw NotArrayException { }; }
    virtual constexpr vector<int> GetIntVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<bool> GetBoolVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<float> GetFloatVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr vector<std::string> GetStringVector(bool) { throw NotArraryOrMapException { }; }
    virtual constexpr void insert(std::string, const bool) { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, const int)  { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, const double)  { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, const std::string &)  { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, std::string &&)  { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, Value *)  { throw NotObjectException { }; }
    virtual constexpr void insert(std::string, std::unique_ptr<Value> &&) { throw NotObjectException { }; }
    virtual constexpr map<std::string, int> GetIntMap(bool) { throw NotObjectException { }; }
    virtual constexpr map<std::string, bool> GetBoolMap(bool) { throw NotObjectException { }; }
    virtual constexpr map<std::string, float> GetFloatMap(bool) { throw NotObjectException { }; }
    virtual constexpr map<std::string, std::string> GetStringMap(bool) { throw NotObjectException { }; }

    virtual constexpr size_t empty() const { throw NotArraryOrMapException(); }
    virtual constexpr size_t size() const { throw NotArraryOrMapException(); }
    constexpr inline size_t length() const { return size(); }

    virtual constexpr const std::string &GetKey() const { throw NotMemberException(); }
    virtual constexpr const Value &GetValue() const { throw NotMemberException(); }

    virtual constexpr std::string &GetKey() { throw NotMemberException(); }
    virtual constexpr Value &GetValue() { throw NotMemberException(); }

    virtual Iterator begin() { throw NotArraryOrMapException { }; }
    virtual Iterator end() { throw NotArraryOrMapException { }; }
    virtual Iterator_const begin() const { throw NotArraryOrMapException { }; }
    virtual Iterator_const end() const { throw NotArraryOrMapException { }; }

    virtual Reverse_Iterator rbegin() { throw NotArraryOrMapException { }; }
    virtual Reverse_Iterator rend() { throw NotArraryOrMapException { }; }
    virtual Reverse_Iterator_const rbegin() const { throw NotArraryOrMapException { }; }
    virtual Reverse_Iterator_const rend() const { throw NotArraryOrMapException { }; }

    constexpr Value &operator[](size_t index) const { return *atptr(index); }
    constexpr Value &operator[](const std::string &key) const { return *atptr(key); }

    constexpr auto &at(size_t index) const { return *atptr(index); }
    constexpr auto &at(const std::string &key) const { return *atptr(key); }

    static constexpr Value *Parse(Stream &stream);
    static constexpr Value *Parse(const std::string &text) { 
        Stream stream { text };
        return Parse(stream);
    }

protected:
    static constexpr Value *ParseIntegerOrFloat(Stream &stream);

    static constexpr std::string ParseString(Stream &stream) {
        std::string value { };
        while(*stream != '"') {
            if (*stream == '\\') {
                ++stream;
                switch(*stream) {
                case '"':
                case '\\':
                    value.push_back(*stream);
                    ++stream;
                    break;
                case 'b':
                    value.push_back('\b');
                    ++stream;
                    break;
                case 'f':
                    value.push_back('\f');
                    ++stream;
                    break;
                case 'n':
                    value.push_back('\n');
                    ++stream;
                    break;
                case 'r':
                    value.push_back('\r');
                    ++stream;
                    break;
                case 't':
                    value.push_back('t');
                    ++stream;
                    break;
                case 'u': {
                    ++stream;
                    char val { 0 };
                    for(size_t index { 0 }; index < 4; ++index) {
                        auto ch = *stream;
                        val *= 10;
                        if (ch >= '0' && ch <= '9') val += ch - '0';
                        else if (ch >= 'a' && ch <= 'f') val += 10 + ch - 'a';
                        else if (ch >= 'A' && ch <= 'F') val += 10 + ch - 'A';
                        else throw JsonParseException { stream, exception_t::INCORRECT_ESCAPE };
                        ++stream;
                    }
                    value.push_back(val);
                    break;
                }
                default:
                    throw JsonParseException { stream, exception_t::INCORRECT_ESCAPE };
                    break;
                }

            } else {
                value.push_back(*stream);
                ++stream;
            }
        }
        ++stream;
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

    const Value &GetConst() const { return *obj; }

public:
    constexpr Ref(const std::string &text) : obj { Value::Parse(text) } { }
    constexpr Ref(Value *obj) : obj { obj } { }
    constexpr Ref();
    Ref(const Ref &) = delete;

    Ref &operator=(const Ref &) = delete;

    constexpr inline type GetType() const { return obj->GetType(); }
    constexpr inline bool IsNull() const { return obj->IsNull(); }
    constexpr inline bool IsBool() const { return obj->IsBool(); }
    constexpr inline bool IsInteger() const { return obj->IsInteger(); }
    constexpr inline bool IsFloat() const { return obj->IsFloat(); }
    constexpr inline bool IsString() const { return obj->IsString(); }
    constexpr inline bool IsMember() const { return obj->IsMember(); }
    constexpr inline bool IsArray() const { return obj->IsArray(); }
    constexpr inline bool IsObject() const { return obj->IsObject(); }

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

    constexpr inline size_t empty() const { return obj->empty(); }
    constexpr inline size_t size() const { return obj->size(); }
    constexpr inline size_t length() const { return obj->length(); }

    constexpr inline const std::string &GetKey() const { return GetConst().GetKey(); }
    constexpr inline const Value &GetValue() const { return GetConst().GetValue(); }
    constexpr inline std::string &GetKey() { return obj->GetKey(); }
    constexpr inline Value &GetValue() { return obj->GetValue(); }


    constexpr inline Iterator begin() { return obj->begin(); }
    constexpr inline Iterator end() { return obj->end(); }
    constexpr inline Iterator_const begin() const { return static_cast<const Value *>(obj.get())->begin(); }
    constexpr inline Iterator_const end() const { return static_cast<const Value *>(obj.get())->end(); }

    constexpr inline Reverse_Iterator rbegin() { return obj->rbegin(); }
    constexpr inline Reverse_Iterator rend() { return obj->rend(); }
    constexpr inline Reverse_Iterator_const rbegin() const { return static_cast<const Value *>(obj.get())->rbegin(); }
    constexpr inline Reverse_Iterator_const rend() const { return static_cast<const Value *>(obj.get())->rend(); }



    constexpr std::string write(const write_format &format) {
        std::string result { };
        write_format_data dataformat {{}, true, format };
        obj->write(result, dataformat);
        return result;
    }

private:
    constexpr Value &QueryInternal(const std::string &text, const auto &delimiter) const;

public:
    constexpr inline Value &Query(const std::string &text, const auto &delimiter) { return QueryInternal(text, delimiter); }
    constexpr inline const Value &Query(const std::string &text, const auto &delimiter) const { return QueryInternal(text, delimiter); }
    constexpr inline Value &Query(const std::string &text) { return QueryInternal(text, '/'); }
    constexpr inline const Value &Query(const std::string &text) const { return QueryInternal(text, '/'); }
    template <typename ChT, const size_t size>
    constexpr Value &Query(const ChT (&text)[size + 1], const auto &delimiter) { return Query(std::string {text, size}, delimiter); }
    template <typename ChT, const size_t size>
    constexpr const Value &Query(const ChT (&text)[size + 1], const auto &delimiter) const { return Query(std::string {text, size}, delimiter); }
    template <typename ChT, const size_t size>
    constexpr Value &Query(const ChT (&text)[size + 1]) { return Query(std::string {text, size}, '/'); }
    template <typename ChT, const size_t size>
    constexpr const Value &Query(const ChT (&text)[size + 1]) const { return Query(std::string {text, size}, '/'); }
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
    
    static constexpr Value *Parse(Stream &stream) {
        auto value = ParseString(stream);
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

class ArrayIterator : public Iterator_Internal {
    vector<std::unique_ptr<Value>>::iterator itr;

public:
    ArrayIterator(vector<std::unique_ptr<Value>>::iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ArrayIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ArrayIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return (*itr).get(); }
    void Increment() override { ++itr; }
};

class ArrayConstIterator : public Iterator_Internal {
    vector<std::unique_ptr<Value>>::const_iterator itr;

public:
    ArrayConstIterator(vector<std::unique_ptr<Value>>::const_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ArrayConstIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ArrayConstIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return (*itr).get(); }
    void Increment() override { ++itr; }
};


class ArrayReverseIterator : public Iterator_Internal {
    vector<std::unique_ptr<Value>>::reverse_iterator itr;

public:
    ArrayReverseIterator(vector<std::unique_ptr<Value>>::reverse_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ArrayReverseIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ArrayReverseIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return (*itr).get(); }
    void Increment() override { ++itr; }
};

class ArrayConstReverseIterator : public Iterator_Internal {
    vector<std::unique_ptr<Value>>::const_reverse_iterator itr;

public:
    ArrayConstReverseIterator(vector<std::unique_ptr<Value>>::const_reverse_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ArrayConstReverseIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ArrayConstReverseIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return (*itr).get(); }
    void Increment() override { ++itr; }
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

    Iterator begin() override { return Iterator { new ArrayIterator { values.begin() } }; }
    Iterator end() override { return Iterator { new ArrayIterator { values.end() } }; }
    Iterator_const begin() const override { return Iterator_const { new ArrayConstIterator { values.begin() } }; }
    Iterator_const end() const override { return Iterator_const { new ArrayConstIterator { values.end() } }; }

    Reverse_Iterator rbegin() override { return Reverse_Iterator { new ArrayReverseIterator { values.rbegin() } }; }
    Reverse_Iterator rend() override { return Reverse_Iterator { new ArrayReverseIterator { values.rend() } }; }
    Reverse_Iterator_const rbegin() const override { return Reverse_Iterator_const { new ArrayConstReverseIterator { values.rbegin() } }; }
    Reverse_Iterator_const rend() const override { return Reverse_Iterator_const { new ArrayConstReverseIterator { values.rend() } }; }
    

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

    constexpr size_t empty() const override { return values.empty(); }
    constexpr size_t size() const override { return values.size(); }

    constexpr type GetType() const noexcept override { return type::Array; }


    static constexpr Value *Parse(Stream &stream) {
        Array *array = new Array { };
        stream.SkipWS();
        if (*stream != ']') {
            for(;;) {
                auto value = Value::Parse(stream);
                array->values.emplace_back(value);
                if (*stream == ']') break;
                if (*stream != ',') {
                    throw JsonParseException { stream, exception_t::INCORRECT_ARRAY_DELIMITER };
                }
                ++stream;
                stream.SkipWS();
            }
        }
        ++stream;
        return array;
    }
};

class Member : public Value {
    friend class Object;

    std::string key;
    std::unique_ptr<Value> value;

public:
    constexpr Member(const std::string &key, std::unique_ptr<Value> &&value) : key { key }, value { std::move(value) } { }
    constexpr Member(std::string &&key, std::unique_ptr<Value> &&value) : key { std::move(key) }, value { std::move(value) } { }
    constexpr Member(std::string &key, Value *value) : key { key }, value { value } { }
    constexpr Member(std::string &&key, Value *value) : key { std::move(key) }, value { value } { }
    constexpr Member(const std::string &key) : key { key }, value { } { }
    constexpr Member(std::string &&key) : key { std::move(key) }, value { } { }

    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Member *>(&other);
        return key == rhs->key && value->operator==(*rhs->value);
    }
    bool operator==(const Member& rhs) const {
        return key == rhs.key && value->operator==(*rhs.value);
    };

    // Member must forward index to its value
    Value *atptr(size_t index) const override { return value->atptr(index); }
    Value *atptr(const std::string &key) const override { return value->atptr(key); }

    constexpr type GetType() const noexcept override { return type::Member; }

    constexpr const std::string &GetKey() const override { return key; }
    constexpr const Value &GetValue() const override { return *value; }
    constexpr std::string &GetKey() override { return key; }
    constexpr Value &GetValue() override { return *value; }

    constexpr void write(std::string &text, write_format_data &data) const override {
        if (data.format.newline_before_object_member && !data.newline_added) {
            text += '\n';
            text += data.prefix;
            data.newline_added = true;
        }
        text.push_back('"');
        text += key;
        text.push_back('"');
        text.push_back(':');
        if (value->IsObject() || value->IsArray())
            data.newline_added = false;
        else if (data.format.space_after_colon) text.push_back(' ');
        value->write(text, data);
    }
};

struct ObjectLess {
    bool operator()(const std::unique_ptr<Member>& lhs, const std::unique_ptr<Member>& rhs) const {
        return lhs->GetKey() < rhs->GetKey();
    }
};

// unordered_set fails with -fanalyzer
using ObjectStore = std::set<std::unique_ptr<Member>, ObjectLess>;


class ObjectIterator : public Iterator_Internal {
    ObjectStore::iterator itr;

public:
    ObjectIterator(ObjectStore::iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ObjectIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ObjectIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return itr->get(); }
    void Increment() override { ++itr; }
};

class ObjectConstIterator : public Iterator_Internal {
    ObjectStore::const_iterator itr;

public:
    ObjectConstIterator(ObjectStore::const_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ObjectConstIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ObjectConstIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return itr->get(); }
    void Increment() override { ++itr; }
};

class ObjectReverseIterator : public Iterator_Internal {
    ObjectStore::reverse_iterator itr;

public:
    ObjectReverseIterator(ObjectStore::reverse_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ObjectReverseIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ObjectReverseIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return itr->get(); }
    void Increment() override { ++itr; }
};

class ObjectConstReverseIterator : public Iterator_Internal {
    ObjectStore::const_reverse_iterator itr;

public:
    ObjectConstReverseIterator(ObjectStore::const_reverse_iterator &&itr) : itr { std::move(itr) } { }
    bool Equal(const Iterator_Internal *other) override { return itr == dynamic_cast<const ObjectConstReverseIterator *>(other)->itr; }
    bool NotEqual(const Iterator_Internal *other) override { return itr != dynamic_cast<const ObjectConstReverseIterator *>(other)->itr; }
    Value &operator*() override { return **itr; }
    Value *operator->() override { return itr->get(); }
    void Increment() override { ++itr; }
};


class Object : public Value {
    ObjectStore values { };

public:
    constexpr bool operator==(const Value& other) const override { 
        auto rhs = dynamic_cast<const Object *>(&other);
        if (values.size() != rhs->values.size()) return false;
        for(auto &value: values) {
            auto itr = rhs->values.find(value);
            if (itr == std::end(rhs->values)) return false;
            if (*value != **itr) return false;
        }
        return true;
    }
    bool operator==(const Object& other) const = delete;
    bool operator==(const Object& other) = delete;
    
    Iterator begin() override { return Iterator { new ObjectIterator { values.begin() } }; }
    Iterator end() override { return Iterator { new ObjectIterator { values.end() } }; }
    Iterator_const begin() const override { return Iterator_const { new ObjectConstIterator { values.begin() } }; }
    Iterator_const end() const override { return Iterator_const { new ObjectConstIterator { values.end() } }; }

    Reverse_Iterator rbegin() override { return Reverse_Iterator { new ObjectReverseIterator { values.rbegin() } }; }
    Reverse_Iterator rend() override { return Reverse_Iterator { new ObjectReverseIterator { values.rend() } }; }
    Reverse_Iterator_const rbegin() const override { return Reverse_Iterator_const { new ObjectConstReverseIterator { values.rbegin() } }; }
    Reverse_Iterator_const rend() const override { return Reverse_Iterator_const { new ObjectConstReverseIterator { values.rend() } }; }

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
            auto itr = std::begin(*this);
            auto end = std::end(*this);
            for(;;)
            {
                itr->write(text, data);
                ++itr;
                if (itr == end) break;
                text.push_back(',');
                if (data.format.newline_after_comma || data.format.newline_before_object_member) {
                    text += '\n';
                    text += data.prefix;
                    data.newline_added = true;
                } else if (data.format.space_after_comma) text.push_back(' ');
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

    constexpr Value *atptr(const std::string &key) const override {
        auto pair_key = std::make_unique<Member>(key);
        auto itr = values.find(pair_key);
        auto end = std::end(values);
        if (itr == end) return &Error::error;
        return (*itr)->value.get();
    }

    constexpr Value *atptr(size_t index) const override {
        auto key = std::to_string(index);
        return atptr(key);
    }


    constexpr type GetType() const noexcept override { return type::Object; }

    static constexpr void ParseMember(Object *obj, Stream &stream) {
        auto key = ParseString(stream);
        stream.SkipWS();
        if (*stream != ':') throw JsonParseException(stream, exception_t::INCORRECT_OBJECT_MEMBER_SEPARATOR);
        ++stream;
        auto value = Value::Parse(stream);
        if (key.empty()) throw JsonParseException(stream, exception_t::OBJECT_NULL_KEY);
        auto valueitr = obj->atptr(key);
        if (!valueitr->IsError()) throw JsonParseException(stream, exception_t::OBJECT_DUPLICATE_KEY);
        auto member = new Member {std::move(key), value};
        obj->values.emplace(member);
    }

    static constexpr Value *Parse(Stream &stream) {
        stream.SkipWS();
        auto value = new Object { };
        if (*stream != '}') {
            for(;;) {
                if (*stream != '"') throw JsonParseException(stream, exception_t::OBJECT_KEY_STRING_EXPECTED);
                ++stream;
                ParseMember(value, stream);
                if (*stream == '}') break;
                if (*stream != ',') throw JsonParseException(stream, exception_t::INCORRECT_OBJECT_DELIMITER);
                ++stream;
                stream.SkipWS();
                // Allowing commma at the end
                if (*stream == '}') break;
            }
        }
        ++stream;
        return value;
    }

    constexpr void insert(std::string key, const bool value) override {
        auto json = new Bool { value };
        auto member = new Member(key, json);
        values.emplace(member);
    }

    constexpr void insert(std::string key, const int value) override {
        auto json = new Integer { value };
        auto member = new Member(key, json);
        values.emplace(member);

    }

    constexpr void insert(std::string key, const double value) override {
        auto json = new Float { value };
        auto member = new Member(key, json);
        values.emplace(member);
    }

    constexpr void insert(std::string key, const std::string &value) override {
        auto json = new String { value };
        auto member = new Member(key, json);
        values.emplace(member);
    }

    constexpr void insert(std::string key, std::string &&value) override {
        auto json = new String { std::move(value) };
        auto member = new Member(key, json);
        values.emplace(member);
    }

    constexpr void insert(std::string key, Value *json) override {
        auto member = new Member(key, json);
        values.emplace(member);
    }

    constexpr void insert(std::string key, std::unique_ptr<Value> &&json) override {
        auto member = new Member { key, std::move(json) };
        values.emplace(member);
    }

    constexpr vector<int> GetIntVector(bool exclude_non_int = true) override {
        vector<int> ret { };
        if (exclude_non_int) {
            for(auto &value: *this) {
                try {
                    ret.push_back(value.GetValue().GetInt());
                } catch(NotIntegerException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.push_back(value.GetValue().GetInt());
            }
        }
        return ret;
    }

    constexpr vector<bool> GetBoolVector(bool exclude_non_bool = true) override {
        vector<bool> ret { };
        if (exclude_non_bool) {
            for(auto &value: *this) {
                try {
                    ret.push_back(value.GetValue().GetBool());
                } catch(NotBoolException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.push_back(value.GetValue().GetBool());
            }
        }
        return ret;
    }

    constexpr vector<float> GetFloatVector(bool exclude_non_float = true) override {
        vector<float> ret { };
        if (exclude_non_float) {
            for(auto &value: *this) {
                try {
                    ret.push_back(value.GetValue().GetFloat());
                } catch(NotFloatException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.push_back(value.GetValue().GetFloat());
            }
        }
        return ret;
    }

    constexpr vector<std::string> GetStringVector(bool exclude_non_string = true) override {
        vector<std::string> ret { };
        if (exclude_non_string) {
            for(auto &value: *this) {
                try {
                    ret.push_back(value.GetValue().GetStringCopy());
                } catch(NotStringException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.push_back(value.GetValue().GetStringCopy());
            }
        }
        return ret;
    }

    constexpr map<std::string, int> GetIntMap(bool exclude_non_int = true) override {
        map<std::string, int> ret { };
        if (exclude_non_int) {
            for(auto &value: *this) {
                try {
                    ret.emplace(value.GetKey(), value.GetValue().GetInt());
                } catch(NotIntegerException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.emplace(value.GetKey(), value.GetValue().GetInt());
            }
        }
        return ret;
    }

    constexpr map<std::string, bool> GetBoolMap(bool exclude_non_bool = true) override {
        map<std::string, bool> ret { };
        if (exclude_non_bool) {
            for(auto &value: *this) {
                try {
                    ret.emplace(value.GetKey(), value.GetValue().GetBool());
                } catch(NotBoolException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.emplace(value.GetKey(), value.GetValue().GetBool());
            }
        }
        return ret;
    }

    constexpr map<std::string, float> GetFloatMap(bool exclude_non_float = true) override {
        map<std::string, float> ret { };
        if (exclude_non_float) {
            for(auto &value: *this) {
                try {
                    ret.emplace(value.GetKey(), value.GetValue().GetFloat());
                } catch(NotFloatException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.emplace(value.GetKey(), value.GetValue().GetFloat());
            }
        }
        return ret;
    }

    constexpr map<std::string, std::string> GetStringMap(bool exclude_non_string = true) override {
        map<std::string, std::string> ret { };
        if (exclude_non_string) {
            for(auto &value: *this) {
                try {
                    ret.emplace(value.GetKey(), value.GetValue().GetString());
                } catch(NotStringException &) {}
            }
        } else {
            for(auto &value: *this) {
                ret.emplace(value.GetKey(), value.GetValue().GetString());
            }
        }
        return ret;
    }

    constexpr size_t empty() const override { return values.empty(); }
    constexpr size_t size() const override { return values.size(); }
}; // class Object

constexpr Value *Value::ParseIntegerOrFloat(Stream &stream) {
    Stream temp { stream };
    if (*temp == '-' || *temp == '+') ++temp;
    if (*temp < '0' || *temp > '9') throw JsonParseException { temp, exception_t::BAD_NUMBER_FORMAT };
    ++temp;
    // Find text, float, string or error
    bool is_float = false;

    while(temp.remaining_size()) {
        if (*temp == '.') {
            is_float = true;
            break;
        } else if (*temp < '0' || *temp > '9') break;
        ++temp;
    };

    if (is_float) {
        double value { };
        auto ret = std::from_chars(stream.curr(), stream.end(), value);
        stream.UpdateCurr(ret.ptr);
        return new Float { value };
    } else {
        int value { };
        auto ret = std::from_chars(stream.curr(), stream.end(), value);
        stream.UpdateCurr(ret.ptr);
        return new Integer { value };
    }
}

constexpr Value *Value::Parse(Stream &stream) {
    stream.SkipWS();
    switch(*stream) {
    case BeginArray: {
            ++stream;
            auto ret = Array::Parse(stream);
            stream.SkipWS();
            return ret;
        }

    case BeginObject: {
            ++stream;
            auto ret = Object::Parse(stream);
            stream.SkipWS();
            return ret;
        }

    case '-': case '+':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
            auto ret = Value::ParseIntegerOrFloat(stream);
            stream.SkipWS();
            return ret;
        }
    case '"': {
            ++stream;
            auto ret = String::Parse(stream);
            stream.SkipWS();
            return ret;
        }
    case 'f':    
    case 'F': {
            ++stream;
            stream.Match("alse");
            stream.SkipWS();
            return new Bool { false };
        }
    case 't':
    case 'T': {
            ++stream;
            stream.Match("rue");
            stream.SkipWS();
            return new Bool { true };
        }
    case 'n':    
    case 'N': {
            ++stream;
            stream.Match("ull");
            stream.SkipWS();
            return new Null { };
        }
    default:
        throw JsonParseException { stream, exception_t::UNKNOWN_KEYWORD };
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


constexpr Value &Ref::QueryInternal(const std::string &text, const auto &delimiter) const {
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

constexpr inline auto Parse(const std::string &text) { return Ref { text }; }

} // namespace rohit::json