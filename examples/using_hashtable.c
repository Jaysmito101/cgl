#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"



int main()
{
    char buffer[1024];
    CGL_hashtable* table = CGL_hashtable_create(100, 0, 10);

    CGL_hashtable_set_int(table, "age", 16);
    CGL_hashtable_set_int(table, "marks", 99);
    CGL_hashtable_set_int(table, "id", 4484);
    CGL_hashtable_set_float(table, "weight", 68.4877f);
    int t = CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE;
    CGL_hashtable_set_string(table, "name", "Andrew");

    printf("ID : %d\n", CGL_hashtable_get_int(table, "id"));
    printf("Weight : %f\n", CGL_hashtable_get_float(table, "weight"));
    CGL_hashtable_get(table, "name", buffer);
    printf("Name : %s\n", buffer);
    printf("Marks : %d\n", CGL_hashtable_get_int(table, "marks"));
    printf("Age : %d\n", CGL_hashtable_get_int(table, "age"));


    printf("name %s.\n", (CGL_hashtable_exists(table, "name") ? "exists" : "does not exist"));
    printf("year %s.\n", (CGL_hashtable_exists(table, "year") ? "exists" : "does not exist"));
    CGL_hashtable_remove(table, "name");
    printf("name %s.\n", (CGL_hashtable_exists(table, "name") ? "exists" : "does not exist"));
    printf("age %s.\n", (CGL_hashtable_exists(table, "age") ? "exists" : "does not exist"));
    CGL_hashtable_remove(table, "name");

    CGL_hashtable_iterator* iterator = CGL_hashtable_iterator_create(table);

    printf("Entries :- \n");
    while(CGL_hashtable_iterator_next(iterator, buffer, NULL, NULL))
        printf("Key : %s\n", buffer);

    CGL_hashtable_iterator_destroy(iterator);

    CGL_hashtable_destroy(table);

    printf("End\n");

    return 0;
}
