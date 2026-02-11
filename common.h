#ifndef COMMON
#define COMMON

#include <stdint.h>
#include <stdbool.h>

#define SAKE_LIBRARY_PATH "/data/local/tmp/sakeloader/libandroid-sake-lib.so"
#define HOOK_RANDOM false
#define SAKE_MSG_SIZE 20

// state of 8 means error ?
#define CHECK_EQ_8(x) do {                          \
    if ((x) == 8) {                                 \
        printf("\n\n\nFAIL: %s == %d\n",   \
                #x, (x));                           \
        exit(1);                                    \
    }                                               \
} while (0)


typedef struct {
    char data[0x20]; // NOT dec 20 "user message", this is an internal struct
    uint32_t size;
} SakeUserMsg;


#endif /* COMMON */
