#ifndef WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#else
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "addrchange.h"


#ifndef WIN32
ADAPTERSTATE get_adapter_status(uint32_t* p_uAddr, uint32_t* p_uNetmask, char szAdapterName[]) /* doc in header */
{
   ADAPTERSTATE adapterstate = DISCONNECTED;
   struct ifaddrs *ifaddr, *ifa;
   char szPotentialAdapter[MAXADAPTERLEN];
   uint32_t uPotAddr = 0, uPotNetmask =0;
   strcpy(szPotentialAdapter, "(none)");

   if (getifaddrs(&ifaddr) == -1)
   {
      eprintf("getifaddrs failed with error %s\n", strerror(errno));
   }
   else
   {
      int family, s;
      for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
      {
         family = ifa->ifa_addr->sa_family;
         if (family == AF_INET) /* Silently drop all other address families except IPv4 */
         {
            struct sockaddr_in *sin = (struct sockaddr_in*)ifa->ifa_addr;
            uint32_t uAddr = sin->sin_addr.s_addr;
            sin = (struct sockaddr_in*)ifa->ifa_netmask;
            uint32_t uNetmask = sin->sin_addr.s_addr;

            //eprintf("Old: %x/%x, new %x/%x\n", *p_uAddr, *p_uNetmask, uAddr, uNetmask);
            /* We have the target address */
            if ((*p_uAddr == uAddr) &&
                (*p_uNetmask == uNetmask))
            {
               adapterstate = CONNECTED;
               strncpy(szAdapterName, ifa->ifa_name, MAXADAPTERLEN); // Important for the first time this is called
               *p_uAddr = uAddr;
               *p_uNetmask = uNetmask;
               break;
            }
            /* We have an alternate address that is from the same network */
            else if ((*p_uAddr & *p_uNetmask) ==
                     (uAddr & uNetmask))
            {
               char szNewAddress[100], szNewNetmask[100];
               char szOldAddress[100], szOldNetmask[100];
               char *temp = inet_ntoa(*(struct in_addr*)p_uAddr); strcpy(szOldAddress, temp);
               temp = inet_ntoa(*(struct in_addr*)&uAddr);        strcpy(szNewAddress, temp);
               temp = inet_ntoa(*(struct in_addr*)p_uNetmask);    strcpy(szOldNetmask, temp);
               temp = inet_ntoa(*(struct in_addr*)&uNetmask);     strcpy(szNewNetmask, temp);

               /* If this is the same adapter as before, give it preference */
               if (!strncmp(szAdapterName, ifa->ifa_name, MAXADAPTERLEN))
               {
                  adapterstate = DIFFERENT_IP;
               }
               /* Accept another adapter only if no other suitable adapter was found */
               else if (adapterstate == DISCONNECTED)
               {
                  adapterstate = DIFFERENT_ADAPTER;
               }
               strncpy(szPotentialAdapter, ifa->ifa_name, MAXADAPTERLEN);
               uPotNetmask = uNetmask;
               uPotAddr = uAddr;
            }
         }
      }
      freeifaddrs(ifaddr);
   }
   if (adapterstate != CONNECTED)
   {
      *p_uAddr = uPotAddr;
      *p_uNetmask = uPotNetmask;
      if (strcmp(szAdapterName, szPotentialAdapter))
      {
         eprintf("Switched from using adapter %s to %s to maintain connectivity!\n",
               szAdapterName, szPotentialAdapter);
      }
      strncpy(szAdapterName, szPotentialAdapter, MAXADAPTERLEN);
   }
   return adapterstate;
}
#else /* For WIN32 */

#endif /* WIN32 */

void calculate_sleep_time(ADAPTERSTATE as, int *sleeptimesec)
{
   assert(sleeptimesec > 0);
   switch(as)
   {
      case DIFFERENT_IP: // fall thru
      case DIFFERENT_ADAPTER:
      case CONNECTED:
         *sleeptimesec = DEFAULT_SLEEP_TIME_SEC; 
         break;

      case DISCONNECTED:
         if (*sleeptimesec == DEFAULT_SLEEP_TIME_SEC)
         {
            *sleeptimesec = MIN_DISCONNEC_SLEEP_SEC;
         }
         *sleeptimesec = DISCONNEC_SLEEP_INCREMENT * *sleeptimesec;
         if (*sleeptimesec > MAX_DISCONNEC_SLEEP_SEC)
         {
            *sleeptimesec = MAX_DISCONNEC_SLEEP_SEC;
         }
         break;
   }
}

char* connstate_to_string(ADAPTERSTATE as)
{
   static char szConnstate[256];
   switch(as)
   {
      case DISCONNECTED:
         strcpy(szConnstate, "DISCONNECTED");
         break;

      case DIFFERENT_ADAPTER:
         strcpy(szConnstate, "DIFFERENT_ADAPTER");
         break;

      case DIFFERENT_IP:
         strcpy(szConnstate, "DIFFERENT_IP");
         break;

      case CONNECTED:
         strcpy(szConnstate, "CONNECTED");
         break;

   }
   return szConnstate;
}

int get_config_options(uint32_t *uIP, uint32_t *uNetmask, uint32_t *uServerIP, uint16_t *uServerPort, 
      char szUsername[], char szPassword[], char szDomain[]) /* Return 0 on success */
{
   int iRet = -1;
   char szIP[20], szNetmask[20], szServerIP[20];
   struct in_addr in;
   printf("Please enter the adapter IP and netmask of the adapter on which to listen\n");
   printf("IP: ");
   scanf("%s", szIP);
   if (inet_aton(szIP, &in) != 0)
   {
      *uIP = (in.s_addr);
      printf("Netmask: ");
      scanf("%s", szNetmask);
      if (inet_aton(szNetmask, &in) != 0)
      {
         *uNetmask = (in.s_addr);
         printf("DynDNS Server IP: ");
         scanf("%s", szServerIP);
         if (inet_aton(szServerIP, &in) != 0)
         {
            int iServerPort;
            printf("DynDNS Server Port: ");
            scanf("%d", &iServerPort);
            if ((iServerPort >= 1) && (iServerPort <= 65535))
            {
               *uServerPort = iServerPort;
               printf("Username: ");
               scanf("%s", szUsername);
               printf("Password : ");
               scanf("%s", szPassword);
               printf("Domain : ");
               scanf("%s", szDomain);
               iRet = 0;
            }
            else
            {
               eprintf("Server Port %d is not valid!\n",iServerPort);
            }
         }
         else
         {
            eprintf("Server IP %s is not valid!\n", szServerIP);
         }
      }
      else
      {
         eprintf("Netmask %s is not valid!\n", szNetmask);
      }
   }
   else
   {
      eprintf("IP address %s is not valid!\n", szIP);
   }
   return iRet;
}

int main()
{
   ADAPTERSTATE adapterstate = DISCONNECTED;
   char szAdapterName[MAXADAPTERLEN];
   uint32_t ip, netmask;
   char szUsername[100], szPassword[100], szDomain[100];
   int sleeptimesec = DEFAULT_SLEEP_TIME_SEC;
   uint32_t uServerIP; uint16_t uServerPort;
   strcpy(szAdapterName, "(none)");

   int iRet = get_config_options(&ip, &netmask, &uServerIP, &uServerPort,
         szUsername, szPassword, szDomain);
   if (0 == iRet)
   {
      adapterstate = get_adapter_status(&ip, &netmask, szAdapterName);
      if (adapterstate != DISCONNECTED)
      {
         printf("Started daemon on adapter %s!\n", szAdapterName);

         uint32_t extIP = 0;
         if (get_external_ip(&extIP) == 0)
         {
            make_http_request(0, htons(80),extIP, ip, szUsername, szPassword, szDomain);
         }
         while (1)
         {
            calculate_sleep_time(adapterstate, &sleeptimesec);
            sleep(sleeptimesec);

            /* Try getting external IP. If this fails, there is no reason to check adapter
               for connectivity etc */
            if (get_external_ip(&extIP) != 0)
            {
               eprintf("Failed to get external IP!\n");
               continue;
            }
            //eprintf("Got an external ip of %x\n", extIP);

            /* Get internal IP */
            adapterstate = get_adapter_status(&ip, &netmask, szAdapterName);
            if ((adapterstate == DIFFERENT_IP) || (adapterstate == DIFFERENT_ADAPTER))
            {
               make_http_request(0, htons(80),extIP, ip, szUsername, szPassword, szDomain);
               eprintf("Adapter state changed from connected to %s\n", connstate_to_string(adapterstate));
            }
         }
      }
      else
      {
         printf("Please enter an IP address for an interface on this machine!\n");
         eprintf("State: %s\n", connstate_to_string(adapterstate));
      }
   }
   return iRet;
}
