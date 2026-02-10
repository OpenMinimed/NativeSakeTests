#include "server.h"
#include "utils.h"
#include "common.h"

static SakeServer_Init_t SakeServer_Init = NULL;
static SakeNewServer_t SakeNewServer = NULL;
static SakeServerHandshake_t SakeServerHandshake = NULL;

static void *server_handle;
static int handshake_step = 0;

int server_handshake(SakeMsg* msg_in, SakeMsg* msg_out) {
	printf("\n\nSERVER STEP %d\n", handshake_step);

    print_sake_msg("\tmsg in = ", msg_in);

    int retval = SakeServerHandshake(0xAAAAAAAA, 0xBBBBBBBB, server_handle, 0xDDDDDDDD, msg_in, 0xFFFFFFFF, msg_out, 0x11111111);
    char* retval_str = sake_handshake_status_str(retval);
    printf("\tretval = %s\n", retval_str);
    
    print_sake_msg("\tmsg out = ", msg_out);

	server_print_status();
    handshake_step++;
    return retval;
}

void server_print_status()
{
    int server_state = *((uint32_t *)server_handle);
    int last_error = *((uint32_t *)(server_handle + 0xa4));
    const char *last_err_str = sake_last_error_str(last_error);

    
    printf("\tserver_state = %d\n", server_state);
    printf("\tlast_err = %s\n" , last_err_str);

    CHECK_EQ_8(server_state);

    return;
}

void server_init(void *hLib, void* key_db)
{
    SakeServer_Init = (SakeServer_Init_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_Sake_1Server_1Init");
    SakeNewServer = (SakeNewServer_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_new_1SAKE_1SERVER_1S");
    SakeServerHandshake = (SakeServerHandshake_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_Sake_1Server_1Handshake");

	server_handle = SakeNewServer(0xAAAAAAAA, 0xBBBBBBBB);
	printf("[i] New sake server is at %p\n", server_handle);

	SakeServer_Init(0xAAAAAAAA, 0xBBBBBBBB, server_handle, 0xDDDDDDDD, key_db, 0xFFFFFFFF);
	printf("[+] Server initialized successfully with keydb\n");
	server_print_status();

    return;
}