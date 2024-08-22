/////////////////////////////////////////////////////////////////////////////////////////////
// Author: Rohit Jairaj Singh (rohit@singh.org.in)                                         //
// LICENSE: GPL-3.0                                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <format>
#include <charconv>

// This is based on https://www.rfc-editor.org/rfc/rfc8259

namespace rohit::json {


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
    ERROR_T_ENTRY(INCORRECT_OBJECT_DELIMITER, "Incorrect object delimiter") \
    LIST_DEFINITION_END

enum class exception_t {
#define ERROR_T_ENTRY(x, y) x,
        ERROR_T_LIST
#undef ERROR_T_ENTRY
};

constexpr std::string_view to_string_view(const exception_t &val) {
    const std::string_view exception_string[] = {
    #define ERROR_T_ENTRY(x, y) std::string(y, sizeof(y) - 1),
        ERROR_T_LIST
    #undef ERROR_T_ENTRY
    };
    
    return exception_string[static_cast<size_t>(val)];
}

class Exception : public std::exception {
protected:
    const exception_t value;

public:
    Exception(const exception_t value) : value { value } { }

    const char* what() const noexcept override { return to_string_view(value).data(); }
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

class JsonParseException : public Exception {
    const char *current_position;
    const size_t buffer_remaining;
public:
    JsonParseException(const char *current_position, const size_t buffer_remaining, const exception_t except)
        : Exception { except }, current_position { current_position }, buffer_remaining { buffer_remaining } { }
    JsonParseException(const JsonParseException &) = delete;
    JsonParseException &operator=(const JsonParseException &) = delete;

    const char* what() const noexcept override { return "Bad Json format"; }

    std::string to_string(const char *start_position) {
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
            errstr += to_string_view(value);
            errstr += "--| ";
        }

        for(size_t index { 0 }; index < std::min(16UL, buffer_remaining); ++index) {
            auto current_ch = current_position[index];
            if (current_ch >= 32 /* &&  current_ch <= 127 */) {
                errstr.push_back(current_ch);
            } else errstr.push_back('#');
        }
        if (buffer_remaining > 16) {
            errstr += " ... more " + std::to_string(buffer_remaining - 16UL) + " characters.";
        }

        return errstr;
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

/*
      begin-array     = ws %x5B ws  ; [ left square bracket

      begin-object    = ws %x7B ws  ; { left curly bracket

      end-array       = ws %x5D ws  ; ] right square bracket

      end-object      = ws %x7D ws  ; } right curly bracket

      name-separator  = ws %x3A ws  ; : colon

      value-separator = ws %x2C ws  ; , comma
*/

class Value {
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
    virtual ~Value() = default;

    virtual type GetType() const noexcept { return type::Error; }

    virtual Value &operator[](size_t) const { throw NotArraryOrMapException { }; }
    virtual Value &operator[](const std::string &) const { throw NotArraryOrMapException { }; }

    virtual bool &GetBool() { throw NotBoolException { }; }
    virtual bool GetBool() const { throw NotBoolException { }; }
    virtual nullptr_t GetNull() const { throw NotNullException { }; }
    virtual int &GetInt() { throw NotIntegerException { }; }
    virtual int GetInt() const { throw NotIntegerException { }; }
    virtual double &GetFloat() { throw NotFloatException { }; }
    virtual double GetFloat() const { throw NotFloatException { }; }
    virtual const std::string_view GetStringView() const { throw NotStringException { }; }
    virtual std::string &GetString() { throw NotStringException { }; }
    virtual const std::string &GetString() const { throw NotStringException { }; }

    static Value *Parse(const char *&text, size_t &size);
    static Value *Parse(const std::string &text) { 
        const char *textptr = text.c_str();
        size_t size = text.size();
        return Parse(textptr, size);
    }

protected:
    template <int sign = 0>
    static constexpr Value *ParseIntegerOrFloat(const char *&text, size_t &size);

    static constexpr std::string ParseString(const char *&text, size_t &size) {
    if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
        auto itr = text;
        std::string value { };
        while(*itr != '"') {
            if (*itr == '\\') {
                ++itr;
                --size;
                if (size < 2) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
                switch(*itr) {
                case '"':
                case '\\':
                    value.push_back(*itr);
                    ++itr; --size;
                    break;
                case 'b':
                    value.push_back('\b');
                    ++itr; --size;
                    break;
                case 'f':
                    value.push_back('\f');
                    ++itr; --size;
                    break;
                case 'n':
                    value.push_back('\n');
                    ++itr; --size;
                    break;
                case 'r':
                    value.push_back('\r');
                    ++itr; --size;
                    break;
                case 't':
                    value.push_back('t');
                    ++itr; --size;
                    break;
                case 'u': {
                    ++itr; --size;
                    if (size < 6) throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                    char val { 0 };
                    auto newsize = size - 4;
                    do {
                        auto ch = *itr;
                        val *= 10;
                        if (ch >= '0' && ch <= '9') val += ch - '0';
                        else if (ch >= 'a' && ch <= 'f') val += 10 + ch - 'a';
                        else if (ch >= 'A' && ch <= 'F') val += 10 + ch - 'A';
                        else throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                        ++itr; --size;
                    } while(size != newsize);
                    value.push_back(val);
                    break;
                }
                default:
                    throw JsonParseException { text, size, exception_t::INCORRECT_ESCAPE };
                    break;
                }

            } else {
                value.push_back(*itr);
                ++itr; --size;
                if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
            }
        }

        return value;
    }
};

class Bool : public Value {
protected:
    bool value;

public:
    Bool(bool value) : value { value } { }

    type GetType() const noexcept override { return type::Bool; }

    bool &GetBool() override { return value; }
    bool GetBool() const override { return value; }
    int GetInt() const override { return static_cast<int>(value); }
    double GetFloat() const override { return static_cast<double>(value); }
};

class Integer : public Value {
    int value;

public:
    Integer(const int value) : value { value } { }

    type GetType() const noexcept override { return type::NumberInt; }

    bool GetBool() const override { return value != 0; }
    int &GetInt() override { return value; }
    int GetInt() const override { return value; }
    double GetFloat() const override { return static_cast<double>(value); }
};

class Float : public Value {
    double value;

public:
    Float(const double value) : value { value } { }

    type GetType() const noexcept override { return type::NumberFloat; }

    bool GetBool() const override { return value != 0; }
    int GetInt() const override { return static_cast<int>(value); }
    double GetFloat() const throw()override { return value; }
    double &GetFloat() override { return value; }

};


class Array : public Value {
    std::vector<std::unique_ptr<Value>> values { };

public:

    Value &operator[](size_t index) const override { 
        if (index >= values.size()) throw ArrayOutOfRangeException { };
        return *values[index];
    }

    Value &operator[](const std::string &key) const override {
        auto index = std::stoul(key);
        if (index >= values.size()) throw ArrayOutOfRangeException { };
        return *values[index];
    }

    type GetType() const noexcept override { return type::Array; }


    static constexpr Value *Parse(const char *&text, size_t &size) {
        if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
        Array *array = new Array { };
        SkipWS(text, size);
        if (*text != ']') {
            for(;;) {
                auto value = Value::Parse(text, size);
                array->values.emplace_back(value);
                if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
                if (*text == ']') break;
                if (*text != ',') {
                    throw JsonParseException { text, size, exception_t::INCORRECT_ARRAY_DELIMITER };
                }
                SkipWS(text, size);
            }
        }
        ++text; --size;
        return array;
    }

};

class String : public Value {
    std::string value;
public:
    String(const std::string &value) : value { value } { }
    String(const std::string &&value) : value { std::move(value) } { }

    const std::string_view GetStringView() const override { return { value.c_str(), value.size() }; }

    std::string &GetString() override { return value; }
    const std::string &GetString() const override { return value; }

    type GetType() const noexcept override { return type::String; }

    // TODO implement escape character
    static constexpr Value *Parse(const char *&text, size_t &size) {
        auto value = ParseString(text, size);
        return new String { std::move(value) };
    }
};


class Object : public Value {
    std::unordered_map<std::string, std::unique_ptr<Value>> values { };
    
public:

    Value &operator[](const std::string &key) const override {
        auto itr = values.find(key);
        if (itr == std::end(values)) throw ObjecctOutOfRangeException { };
        return *itr->second;
    }


    Value &operator[](size_t index) const override { 
        auto key = std::to_string(index);
        auto itr = values.find(key);
        if (itr == std::end(values)) throw ObjecctOutOfRangeException { };
        return *itr->second;
    }

    type GetType() const noexcept override { return type::Object; }

    static constexpr void ParseMember(Object *obj, const char *&text, size_t &size) {
        auto key = ParseString(text, size);
        SkipWS(text, size);
        if (*text != ':') throw JsonParseException(text, size, exception_t::INCORRECT_OBJECT_MEMBER_SEPARATOR);
        ++text; --size;
        SkipWS(text, size);
        auto value = Value::Parse(text, size);
        SkipWS(text, size);
        if (key.empty()) throw JsonParseException(text, size, exception_t::OBJECT_NULL_KEY);
        auto itr = obj->values.find(key);
        if (itr != std::end(obj->values)) throw JsonParseException(text, size, exception_t::OBJECT_DUPLICATE_KEY);
        obj->values.emplace(key, value);
    }

    static constexpr Value *Parse(const char *&text, size_t &size) {
        auto value = new Object { };
        if (*text != '}') {
            for(;;) {
                ParseMember(value, text, size);
                if (*text == '}') break;
                if (*text != ',') throw JsonParseException(text, size, exception_t::INCORRECT_OBJECT_DELIMITER);
                ++text; --size;
                SkipWS(text, size);
            }
        }
        ++text; --size;
        return value;
    }
};

template <const int sign>
constexpr Value *Value::ParseIntegerOrFloat(const char *&text, size_t &size) {
    static_assert(sign == 1 || sign == -1 || sign == 0, "Sign can be 1 or -1 only");
    if (!size) throw JsonParseException { text, size, exception_t::PREMATURE_JSON_TERMINATION };
    auto itr = text;
    auto end = text + size;
    if (*itr <= '0' || *itr >= '9') throw JsonParseException { itr, size, exception_t::BAD_NUMBER_FORMAT };
    ++itr;
    // Find text, float, string or error
    bool is_float = false;

    // TODO: Implement exponent
    while(itr !=end) {
        if (*itr == '.') {
            if (is_float) throw JsonParseException { itr, size, exception_t::BAD_FLOAT_FORMAT };
            is_float = true;
        }
        if (IsWS(*itr) || *itr == ',' || *itr == ']' || *itr == '}') break;
        if (*itr <= '0' || *itr >= '9') throw JsonParseException { itr, size, exception_t::BAD_NUMBER_FORMAT };
        ++itr;
    };

    if (is_float) {
        double value { };
        auto ret = std::from_chars(text, itr, value);
        size -= ret.ptr - itr;
        itr = ret.ptr;
        return new Float { value };
    } else {
        int value { };
        auto ret = std::from_chars(text, itr, value);
        size -= ret.ptr - itr;
        itr = ret.ptr;
        return new Integer { value };
    }
}


Value *Value::Parse(const char *&text, size_t &size) {
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
            ++text; --size;
            auto ret = Value::ParseIntegerOrFloat<-1>(text, size);
            Value::SkipWS(text, size);
            return ret;
        }
    case '+': {
            ++text; --size;
            auto ret = Value::ParseIntegerOrFloat<1>(text, size);
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
    case 'f': {
            if (size < 5) throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
            ++text;
            if (*text++ != 'a') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            if (*text++ != 'l') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            if (*text++ != 's') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            if (*text++ != 'e') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            size -= 5;
            Value::SkipWS(text, size);
            return new Bool { false };
        }
    case 't': {
            if (size < 4) throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD_OR_PREMATURE_TERMINATION };
            ++text;
            if (*text++ != 'r') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            if (*text++ != 'u') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            if (*text++ != 'e') throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
            size -= 4;
            Value::SkipWS(text, size);
            return new Bool { true };
        }
    default:
        throw JsonParseException { text, size, exception_t::UNKNOWN_KEYWORD };
    }
}


} // namespace rohit::json