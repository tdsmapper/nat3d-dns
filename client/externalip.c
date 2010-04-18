#ifndef _MSC_VER
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#else
  #include <Winsock2.h>
  #include <Windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "addrchange.h"
#include "httpinform.h"

#define SERVER "checkip.dyndns.com"

int retrieve_ip(char szResponse[], uint32_t *extIP)
{
   int i = 0;
   int iRet = -1;
   char response[1024];
   memset(response, 0, sizeof(response));
   strncpy(response, szResponse, sizeof(response));
   int numberseen = 0; 
   char *s = strstr(response, "<html>");
   char *r = 0;
   for (; *s; s++)
   {
      if ((*s >= '0') && (*s <= '9'))
      {
         if (!r)
         {
            numberseen = 1;
            r = s;
         }
      }
      else if (*s != '.')
      {
         if (numberseen)
         {
            *s = 0;
            break;
         }
      }
   }
   if (numberseen)
   {
     in_addr_t in = inet_addr(r);
     if ((in != 0) && (in != -1))
     {
       *extIP = in;
       if (iRet)
       {
         iRet = 0;
       }
     }
   }
   return iRet;
}

int get_external_ip(uint32_t *extIP) // return 0 success
{
   int iRet = -1;
   int sockfd;
   struct sockaddr_in serv_addr;
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) 
   {
      iRet = sockfd;
      eprintf("ERROR opening socket\n");
   }
   else
   {
      uint32_t serverip = 0;
      if (get_server_ip(&serverip, SERVER) == 0)
      {
         memset((char *) &serv_addr, 0, sizeof(serv_addr));
         serv_addr.sin_addr.s_addr = serverip;
         serv_addr.sin_family      = AF_INET;
         serv_addr.sin_port        = htons(80);
         if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
         {
            eprintf("ERROR connecting %s\n", strerror(errno));
         }
         else
         {
            char buffer[128];
            char szHttpBody[1024];
            memset(szHttpBody, 0, sizeof(szHttpBody));
            int n;

            sprintf(buffer, "GET checkip.dyndns.com\n\n");
            n = send(sockfd, buffer, sizeof(buffer), 0);
            if (n > 0)
            {
               n = recv(sockfd, szHttpBody, sizeof(szHttpBody), 0);
               if (retrieve_ip(szHttpBody, extIP) == 0)
               {
                  iRet = 0;
               }
               else
               {
                  eprintf("Error extracting IP\n");
               }
            }
            else
            {
               eprintf("HTTP write problem!\n");
            }
         }
      }
      else
      {
         eprintf("Error resolving server %s\n", SERVER);
      }
   }
   return iRet;
}
