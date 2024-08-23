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

### Insertion


## Missed
1. Null object

## Roadmap
1. GetString() for all. As we are returning internal reference this was not possible with other types.
1. XPATH based access
1. Traverser depth first and breath first.
1. Interface to loop all the members to Array and Object.
1. Copy constructor, this will help in isolation of child object.

## Limitation
1. std::map is used for storing JSON object data, although this is not required to be sorted. Even though std::unordered_map is much faster it cannot be used as -fanalyzer reports memory leak with it.