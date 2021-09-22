
#include <yaml.h>
#include "settings.h"

Settings::Settings()
{
}

Settings::~Settings()
{
}

int Settings::load(const char *fnm)
{
FILE *fh = NULL;
yaml_parser_t parser;
yaml_token_t  token;

	if(!yaml_parser_initialize(&parser)) {
		fprintf(stderr, "Failed to initialize YAML parser!\n");
		return -1;
	}
	fh = fopen(fnm, "r");
    if(fh == NULL) {
    	fprintf(stderr, "Failed to open file!\n");
		yaml_parser_delete(&parser);
		return -1;
    }

    yaml_parser_set_input_file(&parser, fh);

	bool key = false;
	bool value = false;
	std::string k,v;
    do {
    	yaml_parser_scan(&parser, &token);
    	switch(token.type) {
    		case YAML_KEY_TOKEN: key = true; break;
    		case YAML_VALUE_TOKEN: value = true; break;
    		case YAML_SCALAR_TOKEN:
    			if(key) {
    				k = (const char *)token.data.scalar.value;
    				key = false;
    			} else if(value) {
    				v = (const char *)token.data.scalar.value;
    				m_Settings[k] = v;
    				printf("[%s] = %s\n", k.c_str(), v.c_str());
    				value = false;
    			}
    			break;

    		default:;
    	}
    	if(token.type != YAML_STREAM_END_TOKEN)
    		yaml_token_delete(&token);
    } while(token.type != YAML_STREAM_END_TOKEN);
    yaml_token_delete(&token);

	yaml_parser_delete(&parser);
	fclose(fh);
	return 0;
}


std::string Settings::get(const char *key)
{
	return m_Settings[key];
}
