#include "config.h"

configuration conf = {
	.line_wrapping = 0,
	.buffer_size = 256
};

char* strip_string(char* str_to_strip) {
	
	char* stripped_str = calloc(LINE_SIZE,sizeof(char));
	
	int i = 0;
	int j = 0;
	int k = 0;
	int stripped_i = 0;
	while(str_to_strip[i] == ' ') i++; // first non-empty index
	while(str_to_strip[j] != '\0') j++; // index of '\0'
	j--;
	while(str_to_strip[j] == ' ') j--; // last non-empty index
	
	while(i<=j) {
		stripped_str[k] = str_to_strip[i];
		k++,i++;
	}
	
	stripped_str[k] = '\0';
	
	return stripped_str;
	
}

bool set_attribute(char* buffer, char* value, enum attribute_type att_type) {
	
	void* real_value;

	if (att_type == INT_ATTR) {
		int* int_ptr = malloc(sizeof(int));
		if (int_ptr == NULL) {
			return EXIT_FAILURE;
		}
		*int_ptr = atoi(value);
		real_value = int_ptr;
	} else if (att_type == CHAR_ATTR) {
		char* char_ptr = malloc(sizeof(char));
		if (char_ptr == NULL) {
			return EXIT_FAILURE;
		}
		*char_ptr = value[0];
		real_value = char_ptr;
	} else if (att_type == BOOL_ATTR) {
		bool* bool_ptr = malloc(sizeof(bool));
		if (bool_ptr == NULL) {
			return EXIT_FAILURE;
		}
		*bool_ptr = (strcmp(value, "true") == 0 || value[0] == '1') ? true : false;
		real_value = bool_ptr;
	} else {
		return EXIT_FAILURE;
	}
	

	int buflasti = 0;
	for(;buffer[buflasti]!='\0';buflasti++);
	
	if (!strncmp(buffer, "LINE_WRAPPING", buflasti)) {
		conf.line_wrapping = *(int*)real_value;
	}
	else if (!strncmp(buffer, "BUFFER_SIZE", buflasti)) {
		conf.buffer_size = *(int*)real_value;
	}
	else {
		return EXIT_FAILURE;
	}
	
	return 1;
}

bool process_line(char* line) {

	int i = 0;
	char* buffer = calloc(LINE_SIZE,sizeof(char));
	char* value = calloc(LINE_SIZE,sizeof(char));

	while (line[i]!=':') {
		buffer[i] = line[i];
		i++;
		if (i == LINE_SIZE-1) { return EXIT_FAILURE;}
	}
	buffer[i] = '\0';
	int buflasti = i;
	int value_i = 0;
	i++;
	while (line[i]!=';') {
		value[value_i] = line[i];
		i++;
		value_i++;
		if (i == LINE_SIZE-1) { return EXIT_FAILURE;}
	}
	value[value_i] = '\0';
	
	buffer = strip_string(buffer);
	value = strip_string(value);
	
	enum attribute_type att_type;
	
	if (!strncmp(buffer, "LINE_WRAPPING", buflasti)) {
		att_type = BOOL_ATTR;
	}
	else if (!strncmp(buffer, "BUFFER_SIZE", buflasti)) {
		att_type = INT_ATTR;
	}
	else {
		return EXIT_FAILURE;
	}
	
	set_attribute(buffer, value, att_type);

}

bool read_config() {
	FILE* file;
	file = fopen("pinov.config","r");

	if(!file) {
		fclose(file);
		fprintf(stderr, "Config file opening failed!\n");
		return EXIT_FAILURE;
	}

	char* line = calloc(LINE_SIZE,sizeof(char));
	
	int i = 0;
	bool is_value = 0;
	char ch;
	while( (ch = getc(file)) != EOF ) {
		
		if (ch == '\n') {
			process_line(line);
			memset(line,0,LINE_SIZE);
			i = 0;
		} else if (i == LINE_SIZE-1){
			fclose(file);
			return EXIT_FAILURE;
		} else {
			line[i] = ch;
			i++;
		}
	}

	return 1;
}


bool write_config() {


return 1;
}
