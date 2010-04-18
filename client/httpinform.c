#ifndef _MSC_VER
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#else
  #include <Winsock2.h>
  #include <Windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "addrchange.h"
#include "httpinform.h"

#ifndef max
int max(int a, int b) {return a>b?a:b;}
#endif
#ifndef min
int min(int a, int b) {return a<b?a:b;}
#endif

void parse_http_return_code(char szRetCode[])
{
   if (!strcmp(szRetCode, "WIN"))
   {
      eprintf("Successful DynDNS update!\n");
   }
   else if (!strcmp(szRetCode, "FAIL"))
   {
      eprintf("General server side failure. Retrying soon!\n");
   }
   else if (!strcmp(szRetCode, "DOMAINFAIL"))
   {
      eprintf("Error with some of your data (wrong domain, wrong IP detection, reached max domains per user) - Visit the DynDNS webpage to learn more!\n");
   }
   else if (!strcmp(szRetCode, "LOGINFAIL"))
   {
      eprintf("Please check your login credentials!\n");
   }
}

int get_server_ip(uint32_t *p_uIP, char szHostname[]) /* Return 0 success */
{ 
   int iRet = 0;
   struct hostent *server;
   server = gethostbyname(szHostname);
   if (server == NULL)
   {
      eprintf("ERROR, no such host %s\n", szHostname);
      iRet = -1;
   }
   memcpy((char *)p_uIP,
         (char *)server->h_addr, 
         min(sizeof(uint32_t), server->h_length));
   return iRet;
}

int get_http_status_code(char szHttpResponse[])
{
   int response = -1;
   char temp[100];
   if (sscanf(szHttpResponse, "%s %d", temp, &response) != 2)
   {
      response = -1;
   }
   return response;
}

int make_request_string(char szRequestString[], int iRequestStringSize, uint32_t uExtIP, uint32_t uIntIP,
      char szUsername[], char szPassword[], char szDomain[])
{
   struct in_addr sin;
   memset(&sin, 0, sizeof(sin));
   sin.s_addr = uExtIP;  /* Host and port in N order */
   char *pszExtIP = inet_ntoa(sin);
   char szExtIP[20];
   strncpy(szExtIP, pszExtIP, sizeof(szExtIP));
   sin.s_addr = uIntIP;  /* Host and port in N order */
   char *pszIntIP = inet_ntoa(sin);
   char szIntIP[20];
   strncpy(szIntIP, pszIntIP, sizeof(szIntIP));

   int iUsernameLength = strlen(szUsername);
   int iPasswordLength = strlen(szPassword);
   int iDomainLength   = strlen(szDomain);
   int iIpLength       = strlen(szIntIP) * 2;
   int extrasLength = iUsernameLength + iPasswordLength + iDomainLength + iIpLength;

   //strcpy(buffer, "GET /dyndns.php?address=123 HTTP/1.1\nhost: localhost\n\n");
   if (iRequestStringSize > extrasLength + 100) /* 100: extras for the HTTP header */
   {
      sprintf(szRequestString, "GET /%s?username=%s&password=%s&domain=%s&extip=%s&intip=%s HTTP/1.1\nhost:%s\n\n", 
            WEBPAGE, szUsername, szPassword, szDomain, szExtIP, szIntIP, HOST);
      return 0;
   }
   else
   {
      eprintf("Bug with application. Buffer size insufficient!\n");
   }
   return -1;
}

int make_http_request(uint32_t uServerIP, uint16_t uServerPort, uint32_t uExtIP, uint32_t uIntIP,  /* All IPs/ports in N order */
      char szUsername[], char szPassword[], char szDomain[])
{
   int iRet = -1;
   int sockfd, n;
   struct sockaddr_in serv_addr;
   char szHttpHeader[1024], szHttpBody[1024];

   memset(szHttpBody, 0, sizeof(szHttpBody));
   memset(szHttpHeader, 0, sizeof(szHttpHeader));
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) 
   {
      iRet = sockfd;
      eprintf("ERROR opening socket\n");
   }
   else
   {
      memset((char *) &serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_addr.s_addr = uServerIP;
      serv_addr.sin_family      = AF_INET;
      serv_addr.sin_port        = uServerPort;
      if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
      {
#ifdef _MSC_VER
        int lasterror = WSAGetLastError();
        eprintf("ERROR connecting %s\n", strerror(errno));
#else
        eprintf("ERROR connecting %s\n", strerror(errno));
#endif
         
      }
      else
      {
         char buffer[2048];
         if (make_request_string(buffer, sizeof(buffer), uExtIP, uIntIP, szUsername, 
                  szPassword, szDomain) == 0)
         {
            n = send(sockfd, buffer, strlen(buffer), 0);
            if (n < 0) 
            {
               eprintf("ERROR writing to socket");
            }
            else
            {
#ifndef _MSC_VER
               FILE *fHttp = fdopen(sockfd, "r");
               if (fHttp)
               {
                  char data[256];
                  int ifHeadOver = 0;
                  while (!feof(fHttp))
                  {
                     fgets(data, sizeof(data), fHttp);
                     /* Empty line marks the end of the header */
                     if (data[0] == '\n' || data[0] == '\r') /* ignore !bHeadOver check */ 
                     {
                        ifHeadOver = 1;
                     }

                     if (ifHeadOver)
                     {
                        strcat(szHttpBody, data);
                     }
                     else
                     {
                        strcat(szHttpHeader, data);
                     }
                  }

                  /* Did we receive the header and body? */
                  if (strlen(szHttpHeader) > 0)
                  {
                     int httpCode = get_http_status_code(szHttpHeader);
                  }
                  else
                  {
                     eprintf("HTTP Header empty!\n");
                  }
               }
               else
               {
                  iRet = -1;
                  eprintf("Error opening file descriptor!\n");
               }
#endif /* _MSC_VER */
               eprintf("HTTP Request success!\n");
            }
         }
      }
   }
   return iRet;
}

