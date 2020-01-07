#include "ObjectDataBase.h"




void print_object_rec(object_db_rec_t *obj_rec, size_t i)
{
    if (!obj_rec)
        return;
    printf(ANSI_COLOR_MAGENTA"-------------------------------------------------------------------------------------------|\n");
    printf(ANSI_COLOR_YELLOW "%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s |\n", 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name); 
    printf(ANSI_COLOR_MAGENTA "-------------------------------------------------------------------------------------------|\n");

}

void print_object_db(object_db_t *object_db)
{
    if (!object_db)
        return;
    object_db_rec_t *head = object_db->head;
    if (!head) 
        return;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(size_t i = 0; head; head = head->next){
        print_object_rec(head, i++);
    }

}

object_db_rec_t* object_db_look_up(object_db_t *object_db, void *ptr)
{
    if (!object_db || !ptr) 
        return NULL;
    
    object_db_rec_t *head = object_db->head;
    if (!head) 
        return NULL;
    
    for(; head; head = head->next){
        if(head->ptr == ptr)
            return head;
    }
    return NULL;
}

void add_object_to_object_db(object_db_t *object_db, void *ptr, int units, struct_db_rec_t* struct_rec, mld_boolean_t is_root)
{
    object_db_rec_t* obj_rec = object_db_look_up(object_db, ptr);

    assert(!obj_rec);
    
    obj_rec = (object_db_rec_t*)calloc(1, sizeof(object_db_rec_t));

    obj_rec->next = NULL;
    obj_rec->ptr = ptr;
    obj_rec->units = units;
    obj_rec->struct_rec = struct_rec;
    obj_rec->is_visited = MLD_FALSE;
    obj_rec->is_root = is_root;

    object_db_rec_t *head = object_db->head;
        
    if(!head){
        object_db->head = obj_rec;
        obj_rec->next = NULL;
        object_db->count++;
        return;
    }

    obj_rec->next = head;
    object_db->head = obj_rec;
    object_db->count++;

}


void* xcalloc(object_db_t* object_db, char* struct_name, size_t units)
{
    if (!object_db || !struct_name)
        return NULL;

    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);


    void* p = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, p, units, struct_rec, MLD_FALSE);

    return p;
}



static void delete_object_record_from_object_db(object_db_t *object_db, object_db_rec_t *object_rec){

    assert(object_rec);

    object_db_rec_t *head = object_db->head;
    if(head == object_rec){
        object_db->head = object_rec->next;
        free(object_rec);
        return;
    }
    
    object_db_rec_t *prev = head;
    head = head->next;

    while(head){
        if(head != object_rec){
            prev = head;
            head = head->next;
            continue;
        }

        prev->next = head->next;
        head->next = NULL;
        free(head);
        return;
    }
}

void xfree(object_db_t *object_db, void* ptr)
{
    if (!ptr)
        return;
    object_db_rec_t *object_rec = object_db_look_up(object_db, ptr);

    assert(object_rec);

    free(object_rec->ptr);

    object_rec->ptr = NULL;

    delete_object_record_from_object_db(object_db, object_rec);
}

