/**
 * ... Magic headers
 */

#define JSON_TYPE "Content-Type:application/json\r\nContent-Length:"
#define BSON_TYPE "Content-Type:application/bson\r\nContent-Length:"
#define QSON_TYPE "Content-Type:application/qson\r\nContent-Length:"
#define MAGIC_SIZE sizeof(JSON_TYPE)
#define HEADER_SIZE (MAGIC_SIZE+12)
#define OK_MSG    "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\nContent-Length:19\r\n\r\n"
#define HELLO_MSG  "<html>Hello from the engine</html>\r\0"
#define PORT_MSG    "HTTP/1.0 989 No Ports\r\n\r\n"
#define BAD_MSG "HTTP/1.0 200 Not Found\r\n\r\n"
