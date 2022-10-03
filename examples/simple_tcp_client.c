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
