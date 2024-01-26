#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 256
#define CONFIG_COUNT 4

typedef enum {
    INT_TYPE,
    BOOL_TYPE,
    CHAR_TYPE,
    STRING_TYPE
    // Add more types as needed
} ValueType;

typedef struct {
    const char* name;
    ValueType type;
    union {
        int int_value;
        bool bool_value;
        char char_value;
        char string_value[MAX_LINE_SIZE];
        // Add more types as needed
    } value;
} ConfigurationField;

ConfigurationField get_config_field(char* name, ConfigurationField* config_list);
bool read_config(ConfigurationField* config_list);
extern bool write_default_configs(ConfigurationField* config_list);


#endif /* CONFIG_H */
