typedef int bool;
enum {false = 0, true = 1};

#ifdef _DAQ_MAIN_

bool debug;
sem_t emptyBuffers, fullBuffers;
const int extra = 200;  // longwords

#else

extern bool debug;
extern sem_t emptyBuffers, fullBuffers;
extern int extra;

#endif

#ifdef _WARP_USE_SHM_

#define WARP_USE_SHM_BUFFER 1
#define WARP_USE_SHM_DATA 1

// #undef WARP_USE_SHM_BUFFER
// #undef WARP_USE_SHM_DATA

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <errno.h>

#ifdef WARP_USE_SHM_BUFFER

#define WARP_SHMBUF_SEM_KEY 1940345
#define WARP_SHMBUF_SHM_KEY 4032913

#endif

#ifdef WARP_USE_SHM_DATA

#define SHM_DATA_SIZE 1000
#define WARP_SHMDAT_SEM_KEY 1940346
#define WARP_SHMDAT_SHM_KEY 4032914

#endif

#ifdef _DAQ_MAIN_

#ifdef WARP_USE_SHM_BUFFER

int shmbuf_semid, shmbuf_shmid;
void* shmbuf_address;

#endif

#ifdef WARP_USE_SHM_DATA

int shmdat_semid, shmdat_shmid;
void* shmdat_address;

#endif

struct sembuf operation[2];

#else

#ifdef WARP_USE_SHM_BUFFER

extern int shmbuf_semid, shmbuf_shmid;
extern void* shmbuf_address;

#endif

#ifdef WARP_USE_SHM_DATA

extern int shmdat_semid, shmdat_shmid;
extern void* shmdat_address;

#endif

extern struct sembuf operation[2];

#endif

#endif
