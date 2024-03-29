#ifndef _HTTPINFORM_H__
#define  _HTTPINFORM_H__
#include "types.h"

#define WEBPAGE "ddns/uclient/"
#define HOST    "localhost"

void parse_http_return_code(char szRetCode[]);

int make_request_string(char szRequestString[], int iRequestStringSize, uint32_t uExtIP, uint32_t uIntIP,
             char szUsername[], char szPassword[], char szDomain[]);

int make_http_request(uint32_t uServerIP, uint16_t uServerPort, uint32_t uExtIP, uint32_t uIntIP,  /* All IPs/ports in N order */
             char szUsername[], char szPassword[], char szDomain[]);

int get_server_ip(uint32_t *p_uIP, char szHostname[]); /* Return 0 success */



#endif /* _HTTPINFORM_H__ */

