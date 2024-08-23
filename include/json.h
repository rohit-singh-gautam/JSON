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
#include <map>
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
    ERROR_T_ENTRY(OBJECT_KEY_STRING_EXPECTED, "Object key is expected to be string") \
    ERROR_T_ENTRY(INCORRECT_OBJECT_DELIMITER, "Incorrect object delimiter") \
    LIST_DEFINITION_END

enum class exception_t {
#define ERROR_T_ENTRY(x, y) x,
        ERROR_T_LIST
#undef ERROR_T_ENTRY
};

constexpr const std::string to_string(const exception_t &val) {
    const std::string exception_string[] = {
    #define ERROR_T_ENTRY(x, y) std::string(y, sizeof(y) - 1),
        ERROR_T_LIST
    #undef ERROR_T_ENTRY
    };
    
    return exception_string[static_cast<size_t>(val)];
}

class Exception : public std::exception {
protected:
    exception_t value;

public:
    Exception(const exception_t value) : value { value } { }
    Exception(Exception &&rhs) : std::exception { std::move(rhs) }, value {rhs.value} { }

    const char* what() const noexcept override { return to_string(value).data(); }
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
            errstr += " ... more " + std::to_string(buffer_remaining - 16UL) + " characters.";
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
    virtual void write(std::string &text) = 0;

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

    static Value *Parse(const char *&text, size_t &size) {
        try {
            return ParseInternal(text, size);
        } catch(JsonParseException &jsonexp) {
            jsonexp.start_position = text;
            throw std::move(jsonexp);
        }
    }

    static Value *Parse(const std::string &text) { 
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

class Bool : public Value {
protected:
    bool value;

public:
    Bool(bool value) : value { value } { }

    void write(std::string &text) override {
        if (value) text += "true";
        else text += "false";
    }

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

    void write(std::string &text) override {
        text += std::to_string(value);
    }

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

    void write(std::string &text) override {
        text += std::to_string(value);
    }

    type GetType() const noexcept override { return type::NumberFloat; }

    bool GetBool() const override { return value != 0; }
    int GetInt() const override { return static_cast<int>(value); }
    double GetFloat() const throw()override { return value; }
    double &GetFloat() override { return value; }

};

class String : public Value {
    std::string value;

public:
    String(const std::string &value) : value { value } { }
    String(const std::string &&value) : value { std::move(value) } { }

    void write(std::string &text) override {
        text.push_back('"');
        EscapeString(value, text);
        text.push_back('"');
    }

    const std::string_view GetStringView() const override { return { value.c_str(), value.size() }; }

    std::string &GetString() override { return value; }
    const std::string &GetString() const override { return value; }

    type GetType() const noexcept override { return type::String; }

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
    std::vector<std::unique_ptr<Value>> values { };

public:
    void write(std::string &text) override {
        if (values.empty()) text += "[]";
        else {
            text.push_back('[');
            auto itr = std::begin(values);
            (*itr)->write(text);
            itr = std::next(itr);
            while(itr != std::end(values)) {
                text.push_back(',');
                (*itr)->write(text);
                itr = std::next(itr);
            }
            text.push_back(']');
        }
    }

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
    std::map<std::string, std::unique_ptr<Value>> values { };
    
public:
    void write(std::string &text) override {
        if (values.empty()) text += "{}";
        else {
            text.push_back('{');
            auto itr = std::begin(values);
            text += itr->first;
            text.push_back(':');
            itr->second->write(text);
            itr = std::next(itr);
            while(itr != std::end(values)) {
                text.push_back(',');
                text += itr->first;
                text.push_back(':');
                itr->second->write(text);
                itr = std::next(itr);
            }
            text.push_back('}');
        }
    }

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
};

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

constexpr inline Value *Parse(const char *&text, size_t &size) { return Value::Parse(text, size); }
constexpr inline Value *Parse(const std::string &text) { return Value::Parse(text); }

} // namespace rohit::json