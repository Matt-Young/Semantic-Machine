/**
 * ... Magic headers
 */

#define JSON_TYPE "POST\r\nContent-Type:text/json\n\rContent-Length:"
#define BSON_TYPE "POST\r\nContent-Type:text/bson\n\rContent-Length:"
#define QSON_TYPE "POST\r\nContent-Type:text/qson\n\rContent-Length:"
#define MAGIC_SIZE sizeof(JSON_TYPE)
#define HEADER_SIZE (MAGIC_SIZE+12)
#define OK_MSG    "HTTP/1.0 200 OK\r\n\r\n"
#define PORT_MSG    "HTTP/1.0 989 No Ports\r\n\r\n"
#define BAD_MSG "HTTP/1.0 404 Not Found\r\n\r\n"
