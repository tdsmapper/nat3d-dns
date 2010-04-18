#ifndef WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <unistd.h>
#else
#include <Winsock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "types.h"
#include "addrchange.h"
#include "httpinform.h"
#include "externalip.h"



#ifndef _MSC_VER
ADAPTER_STATE get_adapter_status(uint32_t* p_uAddr, uint32_t* p_uNetmask, char szAdapterName[]) /* doc in header */
{
   ADAPTER_STATE adapter_state = DISCONNECTED;
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
                (*p_uNetmask == uNetmask) &&
                (*p_uNetmask == uNetmask))
            {
               adapter_state = CONNECTED;
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
                  adapter_state = DIFFERENT_IP;
               }
               /* Accept another adapter only if no other suitable adapter was found */
               else if (ADAPTER_STATE == DISCONNECTED)
               {
                  adapter_state = DIFFERENT_ADAPTER;
               }
               strncpy(szPotentialAdapter, ifa->ifa_name, MAXADAPTERLEN);
               uPotNetmask = uNetmask;
               uPotAddr = uAddr;
            }
         }
      }
      freeifaddrs(ifaddr);
   }
   if (adapter_state != CONNECTED)
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
   return adapter_state;
}
#else /* For WIN32 */
enum ADAPTER_STATE get_adapter_status(uint32_t* p_uAddr, uint32_t* p_uNetmask, char szAdapterName[]) /* doc in header */
{
  enum ADAPTER_STATE adapter_state = DISCONNECTED;
  char szPotentialAdapter[1024];
  uint32_t uPotAddr = 0, uPotNetmask =0;
  uint32_t uAdapterIP = 0, uAdapterMask = 0;
  strcpy(szPotentialAdapter, "(none)");

  PIP_ADAPTER_INFO pAdapterInfo;
  PIP_ADAPTER_INFO pAdapter = NULL;
  DWORD dwRetVal = 0;
  ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
  pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
  if (pAdapterInfo == NULL)
  {
    eprintf("Error allocating memory needed to call GetAdaptersinfo\n");
    adapter_state = GENERIC_ERROR;
  }
  // Make an initial call to GetAdaptersInfo to get
  // the necessary size into the ulOutBufLen variable
  else if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
    if (pAdapterInfo == NULL)
    {
      eprintf("Error allocating memory needed to call GetAdaptersinfo\n");
      adapter_state = GENERIC_ERROR;
    }
  }

  if (((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) &&
    (adapter_state != GENERIC_ERROR)) 
  {
    pAdapter = pAdapterInfo;
    while (pAdapter) 
    {
      uAdapterIP = inet_addr(pAdapter->IpAddressList.IpAddress.String);
      uAdapterMask = inet_addr(pAdapter->IpAddressList.IpMask.String);
      if ((*p_uAddr == uAdapterIP)&&
        (*p_uNetmask == uAdapterMask))
      {
        adapter_state = CONNECTED;
        strncpy(szAdapterName, pAdapter->Description, MAXADAPTERLEN);
        break;
      }
      else if ((*p_uAddr & *p_uNetmask) &&
        (uAdapterIP & uAdapterMask)     && 
        (uAdapterMask == *p_uNetmask))
      {
        if (!strncmp(szAdapterName, pAdapter->Description, MAXADAPTERLEN))
        {
          adapter_state = DIFFERENT_IP;
        }
        else
        {
          adapter_state = DIFFERENT_ADAPTER;
        }
        strncpy(szPotentialAdapter, pAdapter->Description, MAXADAPTERLEN);
        uPotNetmask = uAdapterMask;
        uPotAddr = uAdapterIP;
      }

      pAdapter = pAdapter->Next;
    } 
  }
  else
  {
    printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
  }

  if (adapter_state != CONNECTED)
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
  if (pAdapterInfo)
    free(pAdapterInfo);
  return adapter_state;
}

#endif /* WIN32 */

void calculate_sleep_time(enum ADAPTER_STATE as, int *sleeptimesec)
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
         *sleeptimesec = (int)(DISCONNEC_SLEEP_INCREMENT * *sleeptimesec);
         if (*sleeptimesec > MAX_DISCONNEC_SLEEP_SEC)
         {
            *sleeptimesec = MAX_DISCONNEC_SLEEP_SEC;
         }
         break;
   }
}

char* connstate_to_string(enum ADAPTER_STATE as)
{
   static char szConnstate[256];
   switch(as)
   {
      case DISCONNECTED:
         strncpy(szConnstate, "DISCONNECTED", sizeof(szConnstate));
         break;

      case DIFFERENT_ADAPTER:
         strncpy(szConnstate, "DIFFERENT_ADAPTER", sizeof(szConnstate));
         break;

      case DIFFERENT_IP:
         strncpy(szConnstate, "DIFFERENT_IP", sizeof(szConnstate));
         break;

      case CONNECTED:
         strncpy(szConnstate, "CONNECTED", sizeof(szConnstate));
         break;

   }
   return szConnstate;
}

int get_config_options(uint32_t *uIP, uint32_t *uNetmask, uint32_t *uServerIP, uint16_t *uServerPort, 
      char szUsername[], char szPassword[], char szDomain[]) /* Return 0 on success */
{
   int iRet = -1;
   char szIP[20], szNetmask[20], szServerIP[20];
   in_addr_t in;
   int iServerPort;

   printf("Please enter the adapter IP and netmask of the adapter on which to listen\n");
   printf("IP: ");
   scanf("%s", szIP);
   in = inet_addr(szIP);
   if ((in != 0) || (in != -1)) // diff return types Windows and NIX
   {
      *uIP = in;

      printf("Netmask: ");
      scanf("%s", szNetmask);
      in = inet_addr(szNetmask);
      if ((in != 0) || (in != -1))
      {
         *uNetmask = in;

         printf("DynDNS Server IP: ");
         scanf("%s", szServerIP);
         in = inet_addr(szServerIP);
         if ((in != 0) || (in != -1))
         {
           *uServerIP = in;

            printf("DynDNS Server Port: ");
            scanf("%d", &iServerPort);
            if ((iServerPort >= 1) && (iServerPort <= 65535))
            {
               *uServerPort = htons(iServerPort);
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
  char szAdapterName[MAXADAPTERLEN];
   uint32_t ip, netmask;
   char szUsername[100], szPassword[100], szDomain[100];
   enum ADAPTER_STATE adapter_state = DISCONNECTED;
   
   int sleeptimesec = DEFAULT_SLEEP_TIME_SEC;
   uint32_t uServerIP;
   uint16_t uServerPort;
   int iRet;
   strcpy(szAdapterName, "(none)");

#ifdef _MSC_VER
   WORD wVersionRequested;
   WSADATA wsaData;
   int err;

   /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
   wVersionRequested = MAKEWORD(2, 2);

   err = WSAStartup(wVersionRequested, &wsaData);
   if (err != 0) {
     /* Tell the user that we could not find a usable */
     /* Winsock DLL.                                  */
     printf("WSAStartup failed with error: %d\n", err);
     return 1;
   }
   if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
     /* Tell the user that we could not find a usable */
     /* WinSock DLL.                                  */
     printf("Could not find a usable version of Winsock.dll\n");
     WSACleanup();
     return 1;
   }

#endif

   iRet = get_config_options(&ip, &netmask, &uServerIP, &uServerPort,
         szUsername, szPassword, szDomain);
   if (0 == iRet)
   {
      adapter_state = get_adapter_status(&ip, &netmask, szAdapterName);
      if (adapter_state != DISCONNECTED)
      {
        uint32_t extIP = 0;
         printf("Started daemon on adapter %s!\n", szAdapterName);

         if (get_external_ip(&extIP) == 0)
         {
            make_http_request(uServerIP, uServerPort, extIP, ip, szUsername, szPassword, szDomain);
         }
         while (1)
         {
            calculate_sleep_time(adapter_state, &sleeptimesec);
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
            adapter_state = get_adapter_status(&ip, &netmask, szAdapterName);
            if ((adapter_state == DIFFERENT_IP) || (adapter_state == DIFFERENT_ADAPTER))
            {
               make_http_request(uServerIP, uServerPort, extIP, ip, szUsername, szPassword, szDomain);
               eprintf("Adapter state changed from connected to %s\n", connstate_to_string(adapter_state));
            }
         }
      }
      else
      {
         printf("Please enter an IP address for an interface on this machine!\n");
         eprintf("State: %s\n", connstate_to_string(adapter_state));
      }
   }
   return iRet;
}
