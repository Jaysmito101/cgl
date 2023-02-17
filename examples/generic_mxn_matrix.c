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
#define CGL_EXCLUDE_TEXT_RENDER
#include "cgl.h"



int main()
{
    CGL_init();

    CGL_matrix* mat = CGL_matrix_identity(4, 3);
    CGL_info("Original Matrix");
    CGL_matrix_print(mat);

    CGL_matrix* mat2 = CGL_matrix_submatrix(mat, 0, 0, 3, 3);
    CGL_info("Submatrix");
    CGL_matrix_print(mat2);

    CGL_matrix_set_elem(mat2, 0, 0, 6.0f);
    CGL_info("Submatrix after setting element");
    CGL_matrix_print(mat2);

    CGL_matrix* mat5 = CGL_matrix_copy(mat2);
    CGL_info("Copy of submatrix");
    CGL_matrix_print(mat5);

    CGL_matrix* mat6 = CGL_matrix_transpose(mat2);
    CGL_info("Transpose of submatrix");
    CGL_matrix_print(mat6);    

    CGL_matrix* mat3 = CGL_matrix_mul(mat5, mat6);
    CGL_info("Matrix multiplication");
    CGL_matrix_print(mat3);

    CGL_matrix* mat4 = CGL_matrix_inverse(mat2);
    CGL_info("Inverse of submatrix");
    CGL_matrix_print(mat4);

    CGL_matrix* mat7 = CGL_matrix_mul(mat2, mat4);
    CGL_info("Matrix multiplication");
    CGL_matrix_print(mat7);

    CGL_matrix_destroy(mat);
    CGL_matrix_destroy(mat2);
    CGL_matrix_destroy(mat3);
    CGL_matrix_destroy(mat4);
    CGL_matrix_destroy(mat5);
    CGL_matrix_destroy(mat6);
    CGL_matrix_destroy(mat7);
    CGL_shutdown();
    return 0;
}
