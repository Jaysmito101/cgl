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

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"


static const CGL_byte* s_test_csv = 
"UID,Full Name,Age,Gender,Played For\n"
"1,John Doe,32,Male,3 hrs\n"
"2,Jane Smith,25,Female,1.8 hrs\n"
"3,Rory Johnson,41,Male,2.5 hrs\n"
"4,Amy Lee,19,Female,3.6 hrs\n"
"5,David Brown,27,Male,6.4 hrs\n";

int main()
{
    CGL_init();
    static CGL_byte buffer[1024 * 1024 * 16];
    CGL_info("Hello world!");

    CGL_csv* doc = CGL_csv_create(64);


    CGL_csv_add_column(doc); 
    CGL_csv_add_column(doc);
    CGL_csv_add_column(doc);
    CGL_csv_add_row(doc);
    CGL_info("Added Columns & Rows");

    CGL_csv_set_item(doc, 0, 0, "Year");
    CGL_csv_set_item(doc, 0, 1, "Jan");
    CGL_csv_set_item(doc, 0, 2, "Feb");
    CGL_info("Updated header values");

    CGL_int data_count = 5;

    for (CGL_int i = 0 ; i < data_count ; i++)
    {
        CGL_csv_add_row(doc);
        sprintf(buffer, "%d", 2000 + i); CGL_csv_set_item(doc, i + 1, 0, buffer);
        sprintf(buffer, "%d", CGL_utils_random_int(0, 100)); CGL_csv_set_item(doc, i + 1, 1, buffer);
        sprintf(buffer, "%d", CGL_utils_random_int(0, 100)); CGL_csv_set_item(doc, i + 1, 2, buffer);
    }
    CGL_info("Added data");
    
    CGL_csv_save_to_buffer(doc, buffer, ",");
    CGL_info("Saved");
    CGL_printf_green(buffer);

    CGL_info("Loading CSV");

    CGL_csv_load_from_buffer(doc, s_test_csv, ",");

    CGL_info("Loaded");

    CGL_info("Meta: (%d Columns, %d Rows)", CGL_csv_get_column_count(doc), CGL_csv_get_row_count(doc));
    CGL_info("Headers : ");
    
    for (CGL_int i = 0 ; i < CGL_csv_get_column_count(doc) ; i++)
    {
        CGL_printf_green("%s ", CGL_csv_get_item(doc, 0, i, NULL));
    }
    CGL_printf_green("\n");
    
    CGL_info("Raw CSV (Reformatted): ");
    CGL_csv_save_to_buffer(doc, buffer, "\t");
    CGL_printf_green("%s\n", buffer);

    CGL_shutdown();
}

/*
Sample Output: 

[INTERNAL] [Sat May  6 20:53:50 202] : Started Logger Session
[INFO] [Sat May  6 20:53:50 202] : Hello world!
[INFO] [Sat May  6 20:53:50 202] : Added Columns & Rows
[INFO] [Sat May  6 20:53:50 202] : Updated header values
[INFO] [Sat May  6 20:53:50 202] : Added data
[INFO] [Sat May  6 20:53:50 202] : Saved
Year,Jan,Feb
2000,41,85
2001,72,38
2002,80,69
2003,65,68
2004,96,22
[INFO] [Sat May  6 20:53:50 202] : Loading CSV
[INFO] [Sat May  6 20:53:50 202] : Loaded
[INFO] [Sat May  6 20:53:50 202] : Meta: (5 Columns, 6 Rows)
[INFO] [Sat May  6 20:53:50 202] : Headers :
UID  Full Name  Age  Gender  Played For
[INFO] [Sat May  6 20:53:50 202] : Raw CSV (Reformatted):
UID     Full Name       Age     Gender  Played For
1       John Doe        32      Male    3 hrs
2       Jane Smith      25      Female  1.8 hrs
3       Rory Johnson    41      Male    2.5 hrs
4       Amy Lee         19      Female  3.6 hrs
5       David Brown     27      Male    6.4 hrs

[INTERNAL] [Sat May  6 20:53:50 202] : Ending Logger Session
[INTERNAL] [Sat May  6 20:53:50 202] : Flushed Log Buffer
*/
