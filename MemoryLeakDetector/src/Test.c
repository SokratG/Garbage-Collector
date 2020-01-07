#include <stdio.h>
#include <memory.h>
#include "MLD.h"


typedef struct emp_{
    char emp_name[30];
    size_t emp_id;
    size_t age;
    struct emp_* mgr;
    float salary;
    int *p;
} emp_t;

typedef struct student_{

    char stud_name[32];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_ *best_colleage;
} student_t;

static void testReg();

int main(int argc, char* argv[])
{
    

    testReg();

    return 0;
};


static void testReg()
{
    // Initialize a new structure database
    struct_db_t *struct_db = (struct_db_t*)calloc(1, sizeof(struct_db_t));
    mld_init_primitive_data_types_support(struct_db);

    // Create structure record for structure emp_t
    static field_info_t emp_fields[] = {
        FIELD_INFO(emp_t, emp_name, CHAR,    0),
        FIELD_INFO(emp_t, emp_id,   UINT32,  0),
        FIELD_INFO(emp_t, age,      UINT32,  0),
        FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
        FIELD_INFO(emp_t, salary,   FLOAT,   0),
        FIELD_INFO(emp_t, p,        OBJ_PTR, 0)
    };
    //  Register the structure in structure database
    REG_STRUCT(struct_db, emp_t, emp_fields);

    // save for stud structure
    static field_info_t stud_fields[] = {
        FIELD_INFO(student_t, stud_name, CHAR, 0),
        FIELD_INFO(student_t, rollno,    UINT32, 0),
        FIELD_INFO(student_t, age,       UINT32, 0),
        FIELD_INFO(student_t, aggregate, FLOAT, 0),
        FIELD_INFO(student_t, best_colleage, OBJ_PTR, student_t)
    };
    REG_STRUCT(struct_db, student_t, stud_fields);

    print_structure_db(struct_db);


    // Initialize a new Object database
    object_db_t *object_db = (object_db_t*)calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;
    
    // Create some sample objects
    student_t *Studtest1 = (student_t*)xcalloc(object_db, "student_t", 1);
    set_mld_object_as_global_root(object_db, Studtest1); // root can't leak  
    strncpy(Studtest1->stud_name, "Albert", strlen("Albert"));

    student_t *Studtest2 = (student_t*)xcalloc(object_db, "student_t", 1);
    strncpy(Studtest2->stud_name, "Elise", strlen("Elise")); // this obj is leak because don't free or it's not root

    const size_t units = 2;
    emp_t *Emptest1 = (emp_t*)xcalloc(object_db, "emp_t", units);
    
    //set_mld_object_as_global_root(object_db, Emptest1); //this is root
    strncpy(Emptest1[0].emp_name, "Phill", strlen("Phill"));
    Emptest1[0].emp_id = 22;
    strncpy(Emptest1[1].emp_name, "Scott", strlen("Scott"));

    Emptest1->p = xcalloc(object_db, "int", 1);

    //xfree(object_db, Emptest1->p);

    print_object_db(object_db);
    
    start_mld(object_db);
    report_leaked_objects(object_db);
}
