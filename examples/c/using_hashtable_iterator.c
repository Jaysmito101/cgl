/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
