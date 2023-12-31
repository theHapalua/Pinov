#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_SIZE 256

typedef struct {
	bool line_wrapping;
	int buffer_size;
} configuration;

extern configuration conf;

enum attribute_type {
	INT_ATTR,
	CHAR_ATTR,
	BOOL_ATTR
};

char* strip_string(char* str_to_strip);
bool set_attribute(char* buffer, char* value, enum attribute_type att_type);
bool process_line(char* line);
bool read_config();
bool write_config();


#endif /* CONFIG_H */
