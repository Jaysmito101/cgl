/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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
