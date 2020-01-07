#ifndef __CLASS_DATA_BASE_H__
#define __CLASS_DATA_BASE_H__

#include <assert.h>
#include <stddef.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128
#define MAX_CLASS_NAME_SIZE MAX_STRUCTURE_NAME_SIZE

typedef unsigned long ul;

#define OFFSETOF(struct_name, field_name)     \
    (ul)&(((struct_name *)0)->field_name)

#define FIELD_SIZE(struct_name, field_name) \
     sizeof(((struct_name*)0x00)->field_name)

// Enumeration for data types
typedef enum{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    VOID_PTR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCT  // OBJ_CLASS
} data_type_t;

/* Structure to store the information of one field of a 
 * C structure */
typedef struct _field_info_{
    char fname[MAX_FIELD_NAME_SIZE];    // field name
    data_type_t dtype;                  // data type of the field
    size_t size;                        // size of the field
    size_t offset;                      // offset of the field
    // field is meaningful only if dtype: OBJ_PTR or OBJ_STRUCT
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];
} field_info_t;


/*Structure to store the information of one C structure
 * which could have 'n_fields' fields*/
typedef struct _struct_db_rec_{
    struct _struct_db_rec_* next;                      // Pointer to the next structure in the linked list
    char struct_name[MAX_STRUCTURE_NAME_SIZE];  // key
    size_t ds_size;                             // size of structure 
    size_t n_fields;                            // number of fields
    field_info_t* fields;                       // pointer to array of fields
} struct_db_rec_t;


/*Finally the head of the linked list representing the structure
 * database*/
typedef struct _struct_db_{
    struct_db_rec_t *head;
    unsigned int count;
} struct_db_t;


// Printing functions
void print_structure_rec(struct_db_rec_t *struct_rec);

void print_structure_db(struct_db_t *struct_db);


// Fn to add the structure record in a structure database 

// return 0 on success, -1 on failure for some reason
int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec);

// return pointer to struct if find record about this struct in db, otherwise return NULL
struct_db_rec_t* struct_db_look_up(struct_db_t* struct_db, char* struct_name);


// Structure Registration helping APIs

#define FIELD_INFO(struct_name, fld_name, dtype, nested_struct_name)    \
   {#fld_name, dtype, FIELD_SIZE(struct_name, fld_name),                \
        OFFSETOF(struct_name, fld_name), #nested_struct_name} 

#define REG_STRUCT(struct_db, st_name, fields_arr)                    \
    do{                                                               \
        struct_db_rec_t *rec = (struct_db_rec_t*)calloc(1, sizeof(struct_db_rec_t));    \
        strncpy(rec->struct_name, #st_name, MAX_STRUCTURE_NAME_SIZE); \
        rec->ds_size = sizeof(st_name);                              \
        rec->n_fields = sizeof(fields_arr)/sizeof(field_info_t);     \
        rec->fields = fields_arr;                                    \
        if(add_structure_to_struct_db(struct_db, rec)){              \
            assert(0);                                               \
        }                                                            \
    }while(0);









#ifdef TEST
#endif 

#endif /* __CLASS_DATA_BASE_H__ */
