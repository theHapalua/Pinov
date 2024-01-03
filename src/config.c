#include "config.h"

static void set_config_defaults(ConfigurationField* config_list){
	
	ConfigurationField configs[] = {
		{
        .name = "BACKGROUND_COLOR",
        .type = INT_TYPE,
        .value.int_value = 25
		},
		{
        .name = "LINE_WRAPPING",
        .type = BOOL_TYPE,
        .value.bool_value = 0
		},
		{
		.name = "CHAR_TEST",
		.type = CHAR_TYPE,
		.value.char_value = ' '
		},
		{
		.name = "STRING_TEST",
		.type = STRING_TYPE,
		.value.string_value = "string testing value"
		}
		};

	for(int i=0;i<CONFIG_COUNT;i++) {
		config_list[i] = configs[i];
	}

}

static char* strip_string(char* str_to_strip, char char_to_strip) {
	
	char* stripped_str = calloc(MAX_LINE_SIZE,sizeof(char));
	
	int i = 0;
	int j = 0;
	int k = 0;
	int stripped_i = 0;
	while(str_to_strip[i] == char_to_strip) i++; // first not-expected char index
	while(str_to_strip[j] != '\0') j++; // index of '\0'
	j--;
	while(str_to_strip[j] == char_to_strip) j--; // last non-expected char index
	
	while(i<=j) {
		stripped_str[k] = str_to_strip[i];
		k++,i++;
	}
	
	stripped_str[k] = '\0';
	
	return stripped_str;
	
}

static int get_config_field_index(char *name, ConfigurationField* config_list) {

	int index = -1;
	for(int i=0;i<CONFIG_COUNT;i++) {
		if (!strcmp(name, config_list[i].name))  {
			index = i;
		}
	}
	if (index == -1) return -1; // it means that config field cannot found
	return index;

}


ConfigurationField get_config_field(char* name, ConfigurationField* config_list) { // it sends a copy of requested config field

	int index = get_config_field_index(name, config_list);
	return config_list[index];
}





static bool set_config_field(char* name, char* value, ConfigurationField* config_list) {
	
	int index = get_config_field_index(name, config_list);

	if (config_list[index].type == INT_TYPE) {
		int* int_ptr = malloc(sizeof(int));
		if (int_ptr == NULL) {
			return EXIT_FAILURE;
		}
		
		*int_ptr = atoi(value);
		config_list[index].value.int_value = *int_ptr;
	} else if (config_list[index].type == CHAR_TYPE) {
		char* char_ptr = malloc(sizeof(char));
		if (char_ptr == NULL) {
			return EXIT_FAILURE;
		}
		value = strip_string(value, '\'');
		*char_ptr = *value;
		config_list[index].value.char_value = *char_ptr;
	} else if (config_list[index].type == BOOL_TYPE) {
		bool* bool_ptr = malloc(sizeof(bool));
		if (bool_ptr == NULL) {
			return EXIT_FAILURE;
		}
		*bool_ptr = (strcmp(value, "true") == 0 || value[0] == '1') ? true : false;
		if(!*bool_ptr && (strcmp(value, "false") || value[0] != '0')) return EXIT_FAILURE;
		config_list[index].value.bool_value = *bool_ptr;
	} else if (config_list[index].type == STRING_TYPE) {
		value = strip_string(value, '"');
		for(int i=0;value[i]!='\0';i++) {
			config_list[index].value.string_value[i] = value[i];
		}
	} else {
		return EXIT_FAILURE;
	}
	
	return 1;
}

static bool process_line(char* line, ConfigurationField* config_list) {

	int i = 0;
	char* buffer = calloc(MAX_LINE_SIZE,sizeof(char));
	char* value = calloc(MAX_LINE_SIZE,sizeof(char));

	while (line[i]!=':') {
		buffer[i] = line[i];
		i++;
		if (i == MAX_LINE_SIZE-1) { return EXIT_FAILURE;}
	}
	buffer[i] = '\0';
	int buflasti = i;
	int value_i = 0;
	i++;
	while (line[i]!=';') {
		value[value_i] = line[i];
		i++;
		value_i++;
		if (i == MAX_LINE_SIZE-1) { return EXIT_FAILURE;}
	}
	value[value_i] = '\0';
	
	buffer = strip_string(buffer, ' ');
	value = strip_string(value, ' ');
	
	set_config_field(buffer, value, config_list);

}

bool read_config(ConfigurationField* config_list) {
	set_config_defaults(config_list);
	
	FILE* file;
	file = fopen("pinov.config","r");

	if(!file) {
		write_default_configs(config_list);
		return EXIT_FAILURE;
	}

	char* line = calloc(MAX_LINE_SIZE,sizeof(char));
	
	int i = 0;
	bool is_value = 0;
	char ch;
	while( (ch = getc(file)) != EOF ) {
		
		if (ch == '\n') {
			process_line(line, config_list);
			memset(line,0,MAX_LINE_SIZE);
			i = 0;
		} else if (i == MAX_LINE_SIZE-1){
			fclose(file);
			return EXIT_FAILURE;
		} else {
			line[i] = ch;
			i++;
		}
	}
	
	fclose(file);

	return 1;
}


extern bool write_default_configs(ConfigurationField* config_list) {
	FILE* file;
	file = fopen("pinov.config", "w");
	
	if(!file) return EXIT_FAILURE;
	
	fputs("This file contains config values for pinov text editor\n", file);
	
	for(int i=0;i<CONFIG_COUNT;i++) {
		fputs(config_list[i].name,file);
		fputc(':', file);
		switch (config_list[i].type) {
			case INT_TYPE:
				fprintf(file, "%d", config_list[i].value.int_value);
				break;
			case CHAR_TYPE:
				fputc('\'', file);
				fputc(config_list[i].value.char_value, file);
				fputc('\'', file);
				break;
			case BOOL_TYPE:
				fputs((config_list[i].value.bool_value) ? "true" : "false", file);
				break;
			case STRING_TYPE:
				fputc('"', file);
				fputs(config_list[i].value.string_value,file);
				fputc('"', file);
				break;
		}
		fputc(';', file);
		fputc('\n', file);
	}
	
	fclose(file);

return 1;
}
