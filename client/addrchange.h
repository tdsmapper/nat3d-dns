#ifndef __ADDRCHANGE_H__
#define __ADDRCHANGE_H__
#include "types.h"

enum ADAPTER_STATE {DISCONNECTED, // No network connectivity in given IP range
   DIFFERENT_IP,                  // Same adapter, different IP
   DIFFERENT_ADAPTER,             // Different adapter altogether
   CONNECTED,                     // Connected adapter
   GENERIC_ERROR} ;  

enum ADAPTER_STATE get_adapter_status(uint32_t *uAddr, uint32_t *uNetmask, char* szAdapterName);
void calculate_sleep_time(enum ADAPTER_STATE as, int *sleeptimesec);
char* connstate_to_string(enum ADAPTER_STATE as);

#define MAXADAPTERLEN 256 
#define DEFAULT_SLEEP_TIME_SEC 30
#define MAX_DISCONNEC_SLEEP_SEC 300
#define DISCONNEC_SLEEP_INCREMENT 0.15
#define MIN_DISCONNEC_SLEEP_SEC 10

#ifdef DEBUG
   #define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
   #define eprintf(A, ...) do{}while(0)
#endif /* DEBUG */

#endif /* __ADDRCHANGE_H__ */

