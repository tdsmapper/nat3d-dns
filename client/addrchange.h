#ifndef __ADDRCHANGE_H__
#define __ADDRCHANGE_H__

typedef enum{DISCONNECTED = 0, // No network connectivity in given IP range
   DIFFERENT_IP = 1,         // Same adapter, different IP
   DIFFERENT_ADAPTER,        // Different adapter altogether
   CONNECTED} ADAPTERSTATE;  // Connected adapter

#define DEFAULT_SLEEP_TIME_SEC 30
#define MAX_DISCONNEC_SLEEP_SEC 300
#define DISCONNEC_SLEEP_INCREMENT 0.15
#define MIN_DISCONNEC_SLEEP_SEC 10

ADAPTERSTATE get_adapter_status(uint32_t *uAddr, uint32_t *uNetmask, char szAdapterName[]);
void calculate_sleep_time(ADAPTERSTATE as, int *sleeptimesec);
char* connstate_to_string(ADAPTERSTATE as);

#ifdef WIN32
   
#else
   #define MAXADAPTERLEN 256 
#endif /* WIN32 */

#ifdef DEBUG
   #define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
   #define eprintf(A, ...) do{}while(0)
#endif /* DEBUG */

#endif /* __ADDRCHANGE_H__ */

