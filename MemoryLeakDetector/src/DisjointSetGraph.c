#include "MLD.h"

/*
 * The global object of the application which is not created by xcalloc
 * should be registered with MLD using below 
*/
void mld_register_global_object_as_root(object_db_t *object_db, void *objptr, char *struct_name, size_t units)
{
    struct_db_rec_t* str_rec = struct_db_look_up(object_db->struct_db, struct_name);
    
    assert(str_rec);

    add_object_to_object_db(object_db, objptr, units, str_rec, MLD_TRUE);  
};

/*
 * Application might create an object using xcalloc , but at the same time the object
 * can be root object. Use this API to override the object flags for the object already
 * preent in object db
 */
void set_mld_object_as_global_root(object_db_t *object_db, void *obj_ptr)
{
    object_db_rec_t* obj_rec = object_db_look_up(object_db, obj_ptr);
    assert(obj_rec);

    obj_rec->is_root = MLD_TRUE;
};

// iterate all root object in graph and return them
static object_db_rec_t* get_next_root_object(object_db_t *object_db, object_db_rec_t *starting_from_here)
{

    object_db_rec_t *first = starting_from_here ? starting_from_here->next : object_db->head;

    while(first){
        if(first->is_root)
            return first;
        first = first->next;
    }

    return NULL;
};

// Mark all visited in object false
static void init_mld_algorithm(object_db_t *object_db)
{

    assert(object_db != NULL);

    object_db_rec_t *obj_rec = object_db->head;

    while(obj_rec){
         obj_rec->is_visited = MLD_FALSE;
         obj_rec = obj_rec->next;
    }
};

// This function explore the direct childs of obj_rec and mark
// them visited. Note that obj_rec must have already visted.
static void mld_explore_objects_recursively(object_db_t *object_db, object_db_rec_t *parent_obj_rec)
{

    char* parent_obj_ptr = NULL;
    char* child_obj_ptr = NULL;
    char* child_obj_offset = NULL;
    void* child_obj_address = NULL;
    field_info_t* field_info = NULL;

    object_db_rec_t *child_object_rec = NULL;
    struct_db_rec_t *parent_struct_rec = parent_obj_rec->struct_rec;

    assert(parent_obj_rec->is_visited);

    if(parent_struct_rec->n_fields == 0){
        return;
    }

    for (size_t i = 0; i < parent_obj_rec->units; ++i){

        parent_obj_ptr = (char*)(parent_obj_rec->ptr) + (i * parent_struct_rec->ds_size);

        for (size_t n_fields = 0; n_fields < parent_struct_rec->n_fields; ++n_fields){

            field_info = &parent_struct_rec->fields[n_fields];


            switch(field_info->dtype){
                case UINT8:
                case UINT32:
                case INT32:
                case CHAR:
                case FLOAT:
                case DOUBLE:
                case OBJ_STRUCT:
                    break;
                case VOID_PTR:
                case OBJ_PTR:
                default:
                    ;

                // child_obj_offset is the memory location inside parent object
                // where address of next level object is stored 
                child_obj_offset = parent_obj_ptr + field_info->offset;
                memcpy(&child_obj_address, child_obj_offset, sizeof(void*));


                // child_object_address now stores the address of the next object in the
                // graph. It could be NULL, Handle that as well
                if (!child_obj_address)
                    continue;
                child_object_rec = object_db_look_up(object_db, child_obj_address);

                assert(child_object_rec);

                // Since able to reach this child object "child_object_rec" 
                // from parent object "parent_obj_ptr", mark this
                // child object as visited and explore its children recursively. 
                
                if (!child_object_rec->is_visited){
                    child_object_rec->is_visited = MLD_TRUE;
                    if(field_info->dtype != VOID_PTR)  // Explore next object only when it is not a VOID_PTR
                        mld_explore_objects_recursively(object_db, child_object_rec);
                }

            }

        }

    }

};



// Traverse the graph starting from root objects and 
// mark all reachable nodes as visited
void start_mld(object_db_t *object_db)
{
    // initial graph mark all node as a no visited
    init_mld_algorithm(object_db);

    // Get the first root object from the object db, it could be 
    // present anywhere in object db.
    object_db_rec_t* root_obj = get_next_root_object(object_db, NULL);

    while(root_obj){
        if(root_obj->is_visited){
            // For avoid  in infinite loop.
            // Application Data structures are cyclic graphs
            root_obj = get_next_root_object(object_db, root_obj);
            continue;
        }
        
        // root objects are always reachable since application holds the global
        // variable to it
        root_obj->is_visited = MLD_TRUE;
        
        // Explore all reachable objects from this root_obj recursively
        mld_explore_objects_recursively(object_db, root_obj);

        root_obj = get_next_root_object(object_db, root_obj);
    } 
};


static void mld_dump_object_rec_detail(object_db_rec_t *obj_rec)
{
    if (!obj_rec)
        return;
   
    field_info_t* field_info = NULL;
    size_t num_fields = obj_rec->struct_rec->n_fields;
    size_t units = obj_rec->units, field_index = 0, obj_idx = 0;
        
    for(; obj_idx < units; obj_idx++){
        char *current_object_ptr = (char *)(obj_rec->ptr) + \
                        (obj_idx * obj_rec->struct_rec->ds_size);

        for(field_index = 0; field_index < num_fields; field_index++){
            
            field_info = &obj_rec->struct_rec->fields[field_index];

            switch(field_info->dtype){
                case UINT8:
                case INT32:
                case UINT32:
                    printf("%s[%d]->%s = %d\n", obj_rec->struct_rec->struct_name, obj_idx, field_info->fname, *(int *)(current_object_ptr + field_info->offset));
                    break;
                case CHAR:
                    printf("%s[%d]->%s = %s\n", obj_rec->struct_rec->struct_name, obj_idx, field_info->fname, (char *)(current_object_ptr + field_info->offset));
                    break;
                case FLOAT:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_idx, field_info->fname, *(float *)(current_object_ptr + field_info->offset));
                    break;
                case DOUBLE:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_idx, field_info->fname, *(double *)(current_object_ptr + field_info->offset));
                    break;
                case OBJ_PTR:
                    printf("%s[%d]->%s = %p\n", obj_rec->struct_rec->struct_name, obj_idx, field_info->fname,  (void *)*(int *)(current_object_ptr + field_info->offset));
                    break;
                case OBJ_STRUCT:
                    break;
                default:
                    break;
            }
        }
    }

};

void report_leaked_objects(object_db_t *object_db)
{
    size_t i = 0;
    object_db_rec_t *head;

    printf("Dumping Leaked Objects\n");

    for(head = object_db->head; head; head = head->next){
        if(!head->is_visited){
            print_object_rec(head, i++);
            mld_dump_object_rec_detail(head);
            printf("\n\n");
        }
    }
};



// Support for primitive data types
void mld_init_primitive_data_types_support(struct_db_t *struct_db)
{
    REG_STRUCT(struct_db, int , 0);
    REG_STRUCT(struct_db, float , 0);
    REG_STRUCT(struct_db, double , 0);
};







