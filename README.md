# vanilla-json
Another very simple JSON Parser

## API

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
int json_dump(json_node_t node, const char *filename);

json_type_t json_get_value_type(json_node_t node);
json_node_t json_get_value_of(const char* string, json_node_t node);
json_node_t json_get_value_at(int index, json_node_t node);
char *json_get_string(json_node_t node, char *buffer, jsize_t length);
char *json_get_number(json_node_t node, char *buffer, jsize_t length);
long json_get_integer(json_node_t node, char *buffer, jsize_t length);
double json_get_float(json_node_t node, char *buffer, jsize_t length);
int json_get_bool(json_node_t node, char *buffer, jsize_t length);
void* json_get_null(json_node_t node, char *buffer, jsize_t length);
```

...