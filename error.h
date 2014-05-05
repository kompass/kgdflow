#ifndef ERROR_KGDFLOW_HEADER
#define ERROR_KGDFLOW_HEADER

#define MEMORY_ALLOC_ERROR 1
#define SDL_ERROR 2
#define OUT_OF_BOUNDS_ERROR 3

typedef struct error
{
	int type;
	char *comment;
	struct error *next;
	
} error_t;

extern error_t *g_error_list;

void new_error(int error_type, char *comment);
error_t* get_error_list();



#endif