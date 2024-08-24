# JSON Library
JSON Library includes parser and json creator. std:string can be passed to JSON file and response can be capture in std::string.

This JSON library make sure that JSON Ref object created can be directly used to store configuration. std::vector and std::map can be retrieved from directly from JSON Ref.

JSON Library was implement using https://www.rfc-editor.org/rfc/rfc8259.

## Feature
Following are the key features:
1. JSON library stores different types of data using C++ polymorphism.
1. Low memory footprint.
1. Easy usage to access child object.
1. Each child object is an independent container can be used directly.
1. RAII memory management, hence auto clean up of resource.
1. Implicit type conversion.
1. Constexpr is attempted, it must works once compilers are more mature with constexpr.

## Usage
### Creation
First step is to create JSON object, this can be done using parsing
```cpp
    std::string jsontext { "true" };
    auto json = rohit::json::Parse(jsontext);
```
Above is an example that create bool Json object. Any complex json string can be passed to Parse.

If this library is being used to create json text or file following can be used to create JSON object.
```cpp
    rohit::json::Ref json { };
```
Elements can be added to json objects see below session for how to add it.

### Retrieval
Simple objects can be directly accessed using.
```cpp
    auto value = json.GetBool();
```
Similar to GetBool() there is function GetInt(), GetFloat() and GetString().

More complex hirerachical json can be access as follows
```cpp
    auto value = json["level1"]["level2"]["level3"].GetString();
```

Array can be accessed as follows:
```cpp
    auto value = json["level1"]["level2"][arrayindex].GetInt();
```

String, Int, Bool, Float or Null will be created based on entry in json file. String containing only number can be used to retrieved Int or float. String containing "true", "false" or numbers can be used as boolean.

Below is json example.
```json
{
  "Key1":
  {
    "key11": "Value1",
    "key12": 32,
    "key19": "16",
    "key13":
    [
      0, 1, 2, 3, 4, 5, 6
    ],
    "key14": true
  }
}
```
```"key19": "16", ``` stores 16 as string, though this can be retrieve as below.

```cpp
    auto value = json["Key1"]["key19"].GetInt();
    // value == 16
```
Also, true results is zero(0) and false results in one(1)..

#### Checking if element is present in array or object
Array and Object will return ```rohit::json::Error``` object if object is not found. Any search on it will result in NotArraryOrMapException will result.
So in previous example below will result in exception.
```cpp
    auto value = json["Key1"]["key20"].GetInt();
    auto value = json["Key2"]["key19"].GetInt();
    // both results in NotArraryOrMapException
```
Above results in exception as "key20" and "Key2" is not present.

```cpp
    auto value = json["Key2"]
    auto value = json["Key2"]["key19"]
    // value.IsError() == true
```

### Insertion
Insertion is only possible with Array and Object. Array exposes push_back function and Object exposes insert function.
```cpp
    jsonarray.push_back(10);
    //or
    auto jsonvalue = std::make_unique<rohit::json::Integer>(10);
    jsonarray.push_back(std::move(jsonvalue));
    //or
    auto jsonfloat = new rohit::json::Float { 10.10 };
    jsonarray.push_back(jsonfloat);
```
In example 1 Json integer is emplace in to Array directly.
Second example creates unique pointer that has to be moved into Array.
Last example a pointer is passed to Array.

For non array or non object value can be directly created.

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


## IMPORTANT
1. Once pointer is passed to Array or Object memory is manager by Array or Object. It will be freed on exit, in case this memory if freed manually double free will occur.
2. 

## Roadmap
1. Copy and Move constructor for all the object, this will help in isolation of child object.
1. Traverser depth first and breath first.
1. Interface to loop all the members to Array and Object.

## Limitation
1. std::map is used for storing JSON object data, although this is not required to be sorted. Even though std::unordered_map is much faster it cannot be used as -fanalyzer reports memory leak with it.