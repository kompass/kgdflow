#ifndef ERROR_KGDFLOW_HEADER
#define ERROR_KGDFLOW_HEADER

typedef struct error
{
	int type;
	char *comment;
	struct error *next;
	
} error_t;

extern error_t g_error_list;

void new_error(int error_type, char *comment);
error_t* get_error_list();



#endif