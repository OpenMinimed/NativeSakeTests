#ifndef COMMON
#define COMMON

#include <stdint.h>
#include <stdbool.h>

#define SAKE_LIBRARY_PATH "/data/local/tmp/sakeloader/libandroid-sake-lib.so"
#define HOOK_RANDOM true

// state of 8 means error ?
#define CHECK_EQ_8(x) do {                          \
    if ((x) == 8) {                                 \
        printf("\n\n\nFAIL: %s == %d\n",   \
                #x, (x));                           \
        exit(1);                                    \
    }                                               \
} while (0)


typedef struct {
    char data[0x20];
    uint32_t size;
} SakeMsg;


#endif /* COMMON */
