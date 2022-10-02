#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

int main()
{
    char buffer[1024];
    CGL_hashtable* table = CGL_hashtable_create(1, 0);
    int idata = 0;
    float fdata = 0.0f;
    CGL_hashtable_set_string(table, "Name", "Jaysmito");
    CGL_hashtable_set_int(table, "Age", 18);
    CGL_hashtable_set_float(table, "Float Data", 54584.4584f);

    // print 
    printf("Age : %d\n", CGL_hashtable_get_int(table, "Age"));
    printf("Float Data : %f\n", CGL_hashtable_get_float(table, "Float Data"));
    CGL_hashtable_get(table, "Name", buffer);
    printf("Name : %s\n", buffer);
    CGL_hashtable_set_string(table, "Name", "Jack");
    CGL_hashtable_get(table, "Name", buffer);
    printf("Name 2 : %s\n", buffer);
    
    CGL_hashtable_iterator* iterator = CGL_hashtable_iterator_create(table);

    size_t sz = 0;
    while(CGL_hashtable_iterator_next(iterator, buffer, NULL, &sz))
        CGL_LOG("Key : %s\t\tValue Size: %zu\n", buffer, sz);

    CGL_hashtable_iterator_destroy(iterator);   
    CGL_hashtable_destroy(table);

    
    
    return 0;
}
