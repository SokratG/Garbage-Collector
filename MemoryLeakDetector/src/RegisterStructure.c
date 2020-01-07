#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StructDataBase.h"


#define OK 0
#define FAIL -1

char* DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR","VOID_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};



void print_structure_rec(struct_db_rec_t *struct_rec)
{
    if (!struct_rec)
        return;
    field_info_t *field = NULL;
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|\n" ANSI_COLOR_RESET); 
    printf(ANSI_COLOR_YELLOW "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET, struct_rec->struct_name,struct_rec->ds_size, struct_rec->n_fields);
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|-------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    for (int i = 0; i < struct_rec->n_fields; ++i){
        field = &(struct_rec->fields[i]);
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d| nstrucname = %-17s  |\n",
                i, field->fname, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_str_name);
        printf("  %-20s |", "");
        printf(ANSI_COLOR_CYAN "---------------------------------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }
};


void print_structure_db(struct_db_t *struct_db)
{
    if (!struct_db)
        return;
    printf("printing structure database\n");
    struct_db_rec_t* struct_rec = struct_db->head;
    printf("Amount of Structures Registered = %d\n", struct_db->count);
    int i = 0;
    while(struct_rec){
        printf("structure № : %d (%p)\n", i++, struct_rec);
        print_structure_rec(struct_rec);
        struct_rec = struct_rec->next;
    }
}


int add_structure_to_struct_db(struct_db_t* struct_db, struct_db_rec_t* struct_rec)
{

    if (!struct_rec)
        return FAIL;

    struct_db_rec_t* head = struct_db->head;

    if (!head){
        struct_db->head =  struct_rec;
        struct_rec->next = NULL;
        struct_db->count++;
        return OK;
    }

    // push front
    struct_rec->next = head;
    struct_db->head = struct_rec;
    struct_db->count++;
    return OK;
}


struct_db_rec_t* struct_db_look_up(struct_db_t* struct_db, char* struct_name)
{
    if (!struct_db || !struct_name)
        return NULL;
    
    struct_db_rec_t* head = struct_db->head;

    if (!head)
        return NULL;

    while(head){
        if (!strncmp(head->struct_name, struct_name, MAX_STRUCTURE_NAME_SIZE))
            return head;
        head = head->next;
    }

    return NULL;
}









