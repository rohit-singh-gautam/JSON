# JSON Library
The JSON Library features both a parser and a JSON creator. You can pass a `std::string` to the JSON file to generate a JSON object, and you can extract the content of this JSON using C++ index operators or the Query API. Alternatively, you can use an empty JSON object to build your JSON, with the final result captured in a `std::string`. This library ensures that any JSON Ref object created can be directly used for storing configuration data. You can also retrieve `std::vector` and `std::map` directly from a JSON Ref. The library is implemented based on [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259).

## Features
The key features of the JSON Library include:

1. **Low Memory Footprint**: Designed to be memory efficient.
1. **Easy Child Object Access**: Simplifies the process of accessing child objects.
1. **Independent Child Containers**: Each child object functions as a standalone container.
1. **RAII Memory Management**: Automatically handles resource cleanup.
1. **Implicit Type Conversion**: Supports automatic type conversions.
1. **Future-Ready Constexpr Support**: Attempts to use `constexpr` with the expectation that it will be fully supported as compilers evolve.

## Usage
IMPORTANT: All copy constructor from json objecct is deleted for performance reason. Hence only reference can be retrieved except Parse function. Value must not be referenced.
```cpp
    auto &child = parentjson["child"]; // This is a reference
    auto value = child.GetInt(); // If reference is taken it will result into exception.
```

### Creation
The initial step involves creating a JSON object, which can be accomplished through parsing.
```cpp
    std::string jsontext { "true" };
    auto json = rohit::json::Parse(jsontext);
```
The above example demonstrates the creation of a boolean JSON object. Any complex JSON string can be parsed using the Parse method.

If this library is being utilized to generate JSON text or files, the following methods can be employed to create a JSON object.
```cpp
    rohit::json::Ref json { };
```
You can add elements to JSON objects; see the session below for instructions on how to do this.

### Retrieval
Simple objects can be accessed directly using the following syntax:
```cpp
auto value = json.GetBool();
```
Similarly, functions such as `GetInt()`, `GetFloat()`, and `GetString()` are available for different types. For more complex hierarchical JSON structures, you can access values like this:
```cpp
auto value = json["level1"]["level2"]["level3"].GetString();
```
Arrays can be accessed using:
```cpp
auto value = json["level1"]["level2"][arrayIndex].GetInt();
```
JSON entries will be created as either a String, Int, Bool, Float, or Null, depending on the content in the JSON file. Strings that contain only numbers can be retrieved as either Int or Float. Strings containing "true" or "false", or numerical values, can be interpreted as booleans. 

Here’s an example JSON:
```json
{
  "Key1": {
    "key11": "Value1",
    "key12": 32,
    "key19": "16",
    "key13": [0, 1, 2, 3, 4, 5, 6],
    "key14": true
  }
}
```

In this example, `"key19": "16"` stores the value "16" as a string, but it can be retrieved as an integer like this:
```cpp
auto value = json["Key1"]["key19"].GetInt(); // value == 16
```
Note that `true` is represented as 0 and `false` is represented as 1 in this context.

#### Checking if element is present in array or object
If an array or object is not found, the code will return a `rohit::json::Error` object. Any subsequent search on this will trigger a `NotArrayOrMapException`. For example, in the code snippet below, the exception will be raised:

```cpp
auto value = json["Key1"]["key20"].GetInt();
auto value = json["Key2"]["key19"].GetInt();
// both result in NotArrayOrMapException
```

This exception occurs because `"key20"` and `"Key2"` are not present in the JSON structure.

```cpp
auto &value = json["Key2"];
auto &value = json["Key2"]["key19"];
// value.IsError() == true
```

### Insertion
Insertion is only supported for Array and Object types. The Array class provides the `push_back` function, while the Object class offers the `insert` function.

```cpp
jsonarray.push_back(10); // Example 1
// or
auto jsonvalue = std::make_unique<rohit::json::Integer>(10);
jsonarray.push_back(std::move(jsonvalue)); // Example 2
// or
auto jsonfloat = new rohit::json::Float { 10.10 };
jsonarray.push_back(jsonfloat); // Example 3
// jsonfloat will be deleted by jsonarray
```

In Example 1, a JSON integer is directly inserted into the Array. In Example 2, a unique pointer is created and then moved into the Array. In Example 3, a raw pointer is passed to the Array. For non-array or non-object types, values can be created directly into array or object.

To keep interface simple emplace_back is implemented with push_back.

To insert into object, key must be provided andd insert function must be used.

```cpp
jsonarray.insert("key1", 10); // Example 1
// or
auto jsonvalue = std::make_unique<rohit::json::Integer>(10);
jsonarray.insert("key1", std::move(jsonvalue)); // Example 2
// or
auto jsonfloat = new rohit::json::Float { 10.10 };
jsonarray.insert("key1", jsonfloat); // Example 3
// jsonfloat will be deleted by jsonarray
```

### Query interface
The JSON child can be accessed using the Query interface. One advantage of the Query interface is that if the child is not present, it will return an Error object rather than throwing an exception. In contrast, using the index operator, as mentioned earlier, may result in an exception.

In our previous json example you can query like below.
```cpp
    auto json = rohit::json::Parse(jsontext);
    auto &jsonvalue = json.Query(std::string {"/Key1/key12"});
    // jsonvalue.GetInt() == 32

    auto &jsonvalue1 = json.Query(std::string {"/Key1/Key25"});
    // jsonvalue1.IsError() == true
```

### Iterator
Iterators are implement in Array and Objects only.

Array iterator return the Value of its storage type. In above json example mentioned in [Retrieval](#Retrieval)
```cpp
  auto &jsonarray = json.Query(std::string {"/Key1/key13"});
  for(auto &val: jsonarray) {
    auto int_value = val.GetInt();
    // use int_value it will iterate through 0, 1, 2, 3, 4, 5, 6.
  }
```

For same example object can be iterated as:
```cpp
  auto &jsonobject = json.Query(std::string {"/Key1"});
  for(auto &member: jsonobject) {
    auto &key = member.GetKey();
    auto &value = member.GetValue();
    // You can use key and value here.
  }
```
Object will always return member that is pair of Key and Value.

### Allowed syntax
#### Caps letter
As per RFC https://www.rfc-editor.org/rfc/rfc8259 [Page 6]
Following character is allowed.
```
      value = false / null / true / object / array / number / string

      false = %x66.61.6c.73.65   ; false

      null  = %x6e.75.6c.6c      ; null

      true  = %x74.72.75.65      ; true
```
This library will all both small and capital letters for each of above.
### Comma at the end of Array and object
Following syntax is allowed
```json
{
  "key13":
  [
    0, 1, 2, 3, 4, 5, 6,
  ],
  "key14": true,
}
```
See comma at the end of 6 and end of true. This is not allowed as per RFC, this library allows it.

## Data Types
As per [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259) following data types are implemented:
1. Null,
1. Boolean (True and False),
1. Number - This is divided Integer and Float. RFC requires only Number.
    1. Integer
    1. Float
1. String
1. Array
1. Object
1. Member - This is object member.

Additional type Error is defined to represent object not found in Array or Object.

## IMPORTANT
1. Once pointer is passed to Array or Object memory is manager by Array or Object. It will be freed on exit, in case this memory if freed manually double free will occur.

## Limitation
1. std::map and std::set is used for storing JSON object data, although this is not required to be sorted. Even though std::unordered_map and std::set is much faster it cannot be used as -fanalyzer reports memory leak with it.