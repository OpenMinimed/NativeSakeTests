#include "utils.h"
#include "common.h"

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdbool.h>

void hex_dump(const void *data, int size) {
    if (data == NULL) {
        printf("<NULL>");
        return;
    }
    const char *ptr = (const char *)data;
    for (int i = 0; i < size; i++) {
        printf("%02x ", ptr[i]);
    }
	//printf("\n");
}

void print_sake_msg(const char* prefix, SakeUserMsg* msg) {
    printf("%s",prefix);
    if (msg == NULL) {
        printf("<NULL>\n");
        return;
    }

    hex_dump(msg, msg->size);
    printf("\n");
}

void* get_so_base_addr(const char *so_filename) {
    char maps_filename[256];
    FILE *maps_file;
    char line[256];

    // Open /proc/self/maps to check memory mappings
    snprintf(maps_filename, sizeof(maps_filename), "/proc/self/maps");

    maps_file = fopen(maps_filename, "r");
    if (maps_file == NULL) {
        printf("[-] fopen");
        return NULL;
    }

    while (fgets(line, sizeof(line), maps_file)) {
        // Look for the shared object file in the mapping
        if (strstr(line, so_filename)) {
            // The first part of the line is the memory address range
            // Example line: 7f6f5c800000-7f6f5c81f000 r-xp 00000000 08:01 123456 /path/to/your/library.so
            void *base_addr;
            sscanf(line, "%p", &base_addr);
            fclose(maps_file);
            return base_addr;
        }
    }

    fclose(maps_file);
    printf("Library not found in memory mappings\n");
    exit(-1);
}

void* load_library(const char *path)
{
	void *handle = dlopen((const char *)path, RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "[-] Error loading library: %s\n", dlerror());
		exit(-1);
	}
	printf("[+] Library loaded successfully at %p\n", handle);
	return handle;
}

void *load_function(void *handle, const char *func_name)
{
	void *func = dlsym(handle, (const char *)func_name);
	if (!func) {
		fprintf(stderr, "[-] Error finding %s: %s\n", func_name, dlerror());
		exit(-1);
	}
	printf("[+] %s -> %p\n", func_name, func);
	return func;
}

void debug_break()
{
	pid_t pid = getpid();
	printf("[i] Sending SIGINT signal to process %d\n", pid);

	if (kill(pid, SIGINT) == -1) {
		printf("[-] [-] Error sending signal");
		exit(-1);
	}
}

void* get_random_data(int len) {

	if (HOOK_RANDOM) {
		void* ptr = malloc(len);
		memset(ptr, 0x42, len);
		return ptr;
	}

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        printf("[-] Failed to open /dev/urandom");
        exit(-1);
    }

    void *buffer = malloc(len);
    if (buffer == NULL) {
        printf("[-] Failed to allocate memory");
        close(fd);
        exit(-1);
    }

    int bytesRead = read(fd, buffer, len);
    if (bytesRead != len) {
        printf("[-] Failed to read enough random data");
        free(buffer);
        close(fd);
        exit(-1);
    }

    close(fd);
    return buffer;
}



const char* sake_last_error_str(int code)
{
    switch(code) {
      //  case 0: return "E_SAKE_HANDSHAKE_ERROR_INVALID";
        case 1-1: return "E_SAKE_HANDSHAKE_ERROR_FIRST";
        case 2-1: return "E_SAKE_HANDSHAKE_NO_ERROR";
        case 3-1: return "E_SAKE_HANDSHAKE_INTERFACE_MISUSED";
        case 4-1: return "E_SAKE_HANDSHAKE_SYNCHRONIZATION_INVALID";
        case 5-1: return "E_SAKE_HANDSHAKE_CHALLENGE_NOT_GENERATED";
        case 6-1: return "E_SAKE_HANDSHAKE_CHALLENGE_INVALID";
        case 7-1: return "E_SAKE_HANDSHAKE_SYNCHRONIZATION_RESPONSE_NOT_GENERATED";
        case 8-1: return "E_SAKE_HANDSHAKE_SYNCHRONIZATION_RESPONSE_INVALID";
        case 9-1: return "E_SAKE_HANDSHAKE_DEVICE_TYPE_NOT_SUPPORTED";
        case 10-1: return "E_SAKE_HANDSHAKE_CHALLENGE_RESPONSE_INVALID";
        case 11-1: return "E_SAKE_HANDSHAKE_CHALLENGE_RESPONSE_NOT_GENERATED";
        case 12-1: return "E_SAKE_HANDSHAKE_CHALLENGE_RESPONSE_NOT_RANDOMIZED";
        case 13-1: return "E_SAKE_HANDSHAKE_SESSION_KEY_NOT_DERIVED";
        case 14-1: return "E_SAKE_HANDSHAKE_SESSION_KEY_NOT_RANDOMIZED";
        case 15-1: return "E_SAKE_HANDSHAKE_SESSION_KEY_INVALID";
        case 16-1: return "E_SAKE_HANDSHAKE_PERMIT_NOT_SECURED";
        case 17-1: return "E_SAKE_HANDSHAKE_PERMIT_NOT_PADDED";
        case 18-1: return "E_SAKE_HANDSHAKE_PERMIT_PADDING_INVALID";
        case 19-1: return "E_SAKE_HANDSHAKE_PERMIT_INVALID";
        case 20-1: return "E_SAKE_HANDSHAKE_PERMIT_ISSUED_TO_DIFFERENT_DEVICE";
        case 21-1: return "E_SAKE_HANDSHAKE_PROTOCOL_VERSION_MISMATCH";
        case 22-1: return "E_SAKE_HANDSHAKE_PASSKEY_NOT_COMPLETED";
        case 23-1: return "E_SAKE_HANDSHAKE_PASSKEY_INVALID";
        case 25-1: return "E_SAKE_HANDSHAKE_ERROR_LAST";
        case 24-1: return "E_SAKE_HANDSHAKE_TRANSCRIPT_NOT_UPDATED";
        default: return "E_SAKE_HANDSHAKE_UNKNOWN";
    }
}

const char* sake_handshake_status_str(int code) {
    // SAKE_HANDSHAKE_STATUS_E.java

    switch (code) {
        case 0: return "E_SAKE_HANDSHAKE_SUCCESSFUL";
        case 1: return "E_SAKE_HANDSHAKE_FAILED";
        case 2: return "E_SAKE_HANDSHAKE_IN_PROGRESS";
        default: return "E_SAKE_HANDSHAKE_UNKNOWN";
    }

}