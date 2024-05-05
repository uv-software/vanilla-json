### Another very simple JSON Parser

_Copyright &copy; 2024 Uwe Vogt, UV Software, Berlin (info@uv-software.com)_

# vanilla-json

**JSON** (JavaScript Object Notation) is a lightweight data-interchange format.
It is easy for humans to read and write.
It is easy for machines to parse and generate.

There are plenty of open-source JSON parsers, even in the C programming language.
I needed one in C90 standard with both a permissive and a copyleft license model.
This is what I came up with.

## Interface

```C
typedef enum json_type {
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL,
    JSON_ERROR = (-1)
} json_type_t;
typedef struct json_node *json_node_t;

json_node_t json_read(const char *filename);
void json_free(json_node_t node);
void json_dump(json_node_t node, const char *filename);

json_type_t json_get_value_type(json_node_t node);
json_node_t json_get_value_of(const char* string, json_node_t node);
json_node_t json_get_value_at(int index, json_node_t node);
json_node_t json_get_value_first(json_node_t node);
json_node_t json_get_value_next(json_node_t node);
char *json_get_object_string(json_node_t node);
int json_get_array_index(json_node_t node);
char *json_get_string(json_node_t node, char *buffer, jsize_t length);
char *json_get_number(json_node_t node, char *buffer, jsize_t length);
long json_get_integer(json_node_t node, char *buffer, jsize_t length);
double json_get_float(json_node_t node, char *buffer, jsize_t length);
int json_get_bool(json_node_t node, char *buffer, jsize_t length);
void* json_get_null(json_node_t node, char *buffer, jsize_t length);
```

### Documentation

See header file `vanilla.h` and generate the Doxygen documentation.

### Let´s Make an Example

An example of how to use **vanilla-json** can be found in the folder [`Trial`](https://github.com/uv-software/vanilla-json/blob/main/Trial/main.c).

### Target Platforms

POSIX&reg; compatible operating systems:

1. macOS&reg;
1. Linux&reg;
1. Cygwin&reg;

Windows&reg; operating system:

1. Windows 10 (x86 and x64)
1. Windows 11 (x86 and x64)

## This and That

The JSON data-interchange syntax can be found [here](https://www.json.org/).

### Dual-License

Except where otherwise noted, this work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License
and under the terms of the GNU General Public License v3.0 (or any later version).
You can choose between one of them if you use these portions of this work in whole or in part.

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries. \
POSIX is a registered trademark of the Institute of Electrical and Electronic Engineers, Inc. \
Windows is a registered trademark of Microsoft Corporation in the United States and/or other countries. \
GNU C/C++ is a registered trademark of Free Software Foundation, Inc. \
Linux is a registered trademark of Linus Torvalds. \
Cygwin is a registered trademark of Red Hat, Inc. \
All other company, product and service names mentioned herein may be trademarks, registered trademarks, or service marks of their respective owners.

### Contact

E-Mail: mailto://info@uv-software.com \
Internet: https://www.uv-software.com
