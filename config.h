#ifndef CONFIG_KGDFLOW_HEADER
#define CONFIG_KGDFLOW_HEADER

typedef struct config
{
	int screen_height;
	int screen_width;
} config_t;

config_t* parse_args(int argc, char *argv);
void close_conf(config_t *conf);

#endif