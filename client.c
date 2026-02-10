#include "client.h"
#include "utils.h"
#include "common.h"

static SakeClient_Init_t SakeClient_Init = NULL;
static SakeNewClient_t SakeNewClient = NULL;
static SakeClientHandshake_t SakeClientHandshake = NULL;

static void *client_handle;
static int handshake_step = 0;

int client_handshake(SakeMsg* msg_in, SakeMsg* msg_out) {
	printf("\n\nCLIENT STEP %d\n", handshake_step);

	print_sake_msg("\tmsg in = ", msg_in);


    int retval = SakeClientHandshake(0xAAAAAAAA, 0xBBBBBBBB, client_handle, 0xDDDDDDDD, msg_in, 0xFFFFFFFF, msg_out, 0x11111111);
    char* retval_str = sake_handshake_status_str(retval);
    
    printf("\tretval = %s\n", retval_str);
    print_sake_msg("\tmsg out = ", msg_out);	
    client_check_status();
    handshake_step++;

    return retval;

}


void client_check_status()
{
    int client_state = *((uint32_t *)client_handle);
    int last_error = *((uint32_t *)(client_handle + 0x74));
    char* last_err_str = sake_last_error_str(last_error);

    printf("\tclient_state = %d\n", client_state);
    printf("\tlast_err = %s\n" , last_err_str);

    CHECK_EQ_8(client_state);

    return;
}

void client_init(void *hLib, void* key_db)
{
    SakeClient_Init = (SakeClient_Init_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_Sake_1Client_1Init");
    SakeNewClient = (SakeNewClient_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_new_1SAKE_1CLIENT_1S");
    SakeClientHandshake = (SakeClientHandshake_t)load_function(hLib, "Java_com_medtronic_minimed_sake_SakeJNI_Sake_1Client_1Handshake");

	client_handle = SakeNewClient(0xAAAAAAAA, 0xBBBBBBBB);
	printf("[i] sake client allocated at %p\n", client_handle);

	SakeClient_Init(0xAAAAAAAA, 0xBBBBBBBB, client_handle, 0xDDDDDDDD, key_db, 0xFFFFFFFF);
	printf("[+] client initialized successfully.\n");
	
    client_check_status();

    return;
}