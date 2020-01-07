#include "ObjectDataBase.h"


// API for to register root objects in disjoint graph
void mld_register_global_object_as_root(object_db_t *object_db, void *objptr, char *struct_name, size_t units);


void set_mld_object_as_global_root(object_db_t *object_db, void *obj_ptr);

// start MLD algorithm
void start_mld(object_db_t *object_db);


// Support for primitive data types
void mld_init_primitive_data_types_support(struct_db_t *struct_db);























