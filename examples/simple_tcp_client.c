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
    if(!CGL_net_init()) return EXIT_FAILURE;

    static bool result = false;
    static size_t length = 0;
    static char buffer[1024 * 1024]; // 1 MB buffer to recieve page
    static char* request_text = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n";

    CGL_net_addrinfo* infos = CGL_net_addrinfo_query("www.example.com", "http", NULL);
    CGL_LOG("CGL_net_addrinfo* infos = CGL_net_addrinfo_query(\"example.com\", \"http\", NULL);\n");

    CGL_net_socket* socket = CGL_net_socket_create();
    CGL_LOG("CGL_net_socket* socket = CGL_net_socket_create();\n");

    result = CGL_net_socket_connect(socket, infos);
    CGL_LOG("CGL_net_socket_connect(socket, infos) --> %d\n", result);

    result = CGL_net_socket_send(socket, request_text, strlen(request_text), &length);
    CGL_LOG("CGL_net_socket_send(socket, request_text, strlen(request_text), &length) --> %d\n", result);

    CGL_LOG("Attempted to send %zu bytes\n", strlen(request_text));
    CGL_LOG("Send %zu bytes\n", length);

    result = CGL_net_socket_recv(socket, buffer, 512, &length);
    CGL_LOG("CGL_net_socket_recv(socket, buffer, sizeof(buffer), &length) --> %d\n", result);

    CGL_LOG("Recived %zu bytes\n", length);
    CGL_LOG("Recived Data : \n-----------------------------\n%s\n----------------------\n", buffer);

    CGL_net_addrinfo_destroy(infos);
    CGL_LOG("CGL_net_addrinfo_destroy(infos);\n");

    CGL_net_socket_close(socket);
    CGL_LOG("CGL_net_socket_close(socket);\n");   


    CGL_net_shutdown();
    CGL_LOG("CGL_net_shutdown();\n");

    return EXIT_SUCCESS;
}
