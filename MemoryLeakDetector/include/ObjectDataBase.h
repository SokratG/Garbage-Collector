#ifndef __OBJECT_DATA_BASE_H__
#define __OBJECT_DATA_BASE_H__
#include "StructDataBase.h"


typedef enum{

    MLD_FALSE,
    MLD_TRUE
    
} mld_boolean_t;

typedef struct _object_db_rec_{
    struct _object_db_rec_* next;
    void* ptr; // key
    size_t units;
    mld_boolean_t is_root; // flag check if given object is a root of disjoint graph
    mld_boolean_t is_visited; // flag for traversel graph
    struct_db_rec_t* struct_rec;
} object_db_rec_t;


typedef struct _object_db_{
    struct_db_t* struct_db;
    object_db_rec_t* head;
    size_t count;
} object_db_t;


void print_object_rec(object_db_rec_t *obj_rec, size_t i);

void print_object_db(object_db_t *object_db);


// API to the object:
void* xcalloc(object_db_t* object_db, char* struct_name, size_t units);

void add_object_to_object_db(object_db_t *object_db, void *ptr, int units, struct_db_rec_t* struct_rec, mld_boolean_t is_root);

void xfree(object_db_t *object_db, void* ptr);

object_db_rec_t* object_db_look_up(object_db_t *object_db, void *ptr);

#endif /* __OBJECT_DATA_BASE_H__ */
