#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "server.h"
#include "keydb.h"
#include "socket.h"

void *library_handle = NULL;

char SERVER_KEY_DB[] = {
0xb0, 0x79, 0xcd, 0xc5, 0x04, 0x01, 0x01, 0x44, 0x45, 0x52, 0x49, 0x56, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x5f, 0x5f, 0x4b, 0x45, 0x59, 0x48, 0x4e, 0x44, 0x53, 0x48, 0x4b, 0x45, 0x5f, 0x41, 0x55, 0x54, 0x48, 0x5f, 0x4b, 0x45, 0x59, 0x50, 0x48, 0x4f, 0x4e, 0x45, 0x5f, 0x50, 0x45, 0x52, 0x4d, 0x49, 0x54, 0x5f, 0x45, 0x4e, 0x43, 0x50, 0x48, 0x4f, 0x4e, 0x45, 0x5f, 0x50, 0x45, 0x52, 0x4d, 0x49, 0x54, 0x5f, 0x4d, 0x41, 0x43, 0xad, 0x14, 0xad, 0x27, 0x80, 0x43, 0x7d, 0xb8, 0x92, 0xd5, 0x65, 0x05, 0x67, 0xd4, 0x91, 0xb9
};

char CLIENT_KEY_DB[] = { 
0xdb, 0x8c, 0x1f, 0x28, 0x01, 0x01, 0x04, 0x44, 0x45, 0x52, 0x49, 0x56, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x5f, 0x5f, 0x5f, 0x4b, 0x45, 0x59, 0x48, 0x4e, 0x44, 0x53, 0x48, 0x4b, 0x45, 0x5f, 0x41, 0x55, 0x54, 0x48, 0x5f, 0x4b, 0x45, 0x59, 0x50, 0x55, 0x4d, 0x50, 0x5f, 0x50, 0x45, 0x52, 0x4d, 0x49, 0x54, 0x5f, 0x45, 0x4e, 0x43, 0x52, 0x50, 0x55, 0x4d, 0x50, 0x5f, 0x50, 0x45, 0x52, 0x4d, 0x49, 0x54, 0x5f, 0x43, 0x4d, 0x41, 0x43, 0xf2, 0xf8, 0xdb, 0xbb, 0x51, 0x56, 0x3d, 0x4f, 0xa9, 0x8f, 0xda, 0xff, 0x00, 0x42, 0xa4, 0x32
};

void check_success(int a, int b) {
	if (a == 0 && b == 0) {
		printf("\n\nHANDSHAKE SUCCESS :---)\n");
		exit(0);
	}
}

void server_client_test() {

	// init the key dbs
	char *server_keydb = keydb_init(library_handle, &SERVER_KEY_DB, sizeof(SERVER_KEY_DB));
	char *client_keydb = keydb_init(library_handle, &CLIENT_KEY_DB, sizeof(CLIENT_KEY_DB));

	// init server
	server_init(library_handle, server_keydb);
	printf("\n");

	// init client
	client_init(library_handle, client_keydb);
	printf("\n");

	// create msg buffers
	SakeUserMsg *p_in_server_msg = malloc(sizeof(SakeUserMsg));
	SakeUserMsg *p_out_server_msg = malloc(sizeof(SakeUserMsg));

	SakeUserMsg *p_in_client_msg = malloc(sizeof(SakeUserMsg));
	SakeUserMsg *p_out_client_msg = malloc(sizeof(SakeUserMsg));

	// init both with null ptr, this will internally generate stuff
	server_handshake(NULL, p_out_server_msg);
	client_handshake(NULL, p_out_client_msg);

	// run it
	int server_last = 0;
	int client_last = 0;
	int max_count = 4;

	for (int i = 0; i < max_count; i++ ) {
		server_last = server_handshake(p_out_client_msg, p_out_server_msg);
		check_success(client_last, server_last);
		client_last = client_handshake(p_out_server_msg, p_out_client_msg);
		check_success(client_last, server_last);
	}

	printf("\n\nHANDSHAKE FAILED! :===(\n");

}

void socket_client_test() {

	
	char *client_keydb = keydb_init(library_handle, &CLIENT_KEY_DB, sizeof(CLIENT_KEY_DB));
	
	client_init(library_handle, client_keydb);
	
	
	char* recv_buf = malloc(SAKE_MSG_SIZE);
	char* send_buf = malloc(SAKE_MSG_SIZE);
	
	SakeUserMsg *client_in_buf = malloc(sizeof(SakeUserMsg));
	SakeUserMsg *client_out_buf = malloc(sizeof(SakeUserMsg));
	
	// init it
	client_handshake(NULL, client_out_buf);
	//memset(client_out_buf, sizeof(SakeUserMsg));
	
	int socket_fd = socket_create();

	int tries = 4;
	for (int i = 0; i < tries; i++) {

		
		// get and copy data
		socket_recv(recv_buf);
		memcpy(client_in_buf, recv_buf, SAKE_MSG_SIZE);

		// set struct size
		client_in_buf->size = SAKE_MSG_SIZE;

		// call the lib
		int retval = client_handshake(client_in_buf, client_out_buf);

		// send back the resp
		memcpy(send_buf, client_out_buf, SAKE_MSG_SIZE);
		socket_send(send_buf);

		if (retval == 0) {
			printf("handshake OK!\n");
			break;
		}
	}
	
	socket_close();
	return;
}

int main(int argc, char *argv[])
{

	// set buffer size in case of segfault (for adb shell piping)
	setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

	// check if we are in debug mode based on args
	bool debug = false;
	if (argc == 2)
	{
		if (strcmp((const char *)argv[1], "--debug") == 0)
		{
			debug = true;
			printf("[+] Debug mode enabled.\n");
		}
		else
		{
			printf("[i] Starting in normal mode!\n");
		}
	}

	// load the lib
	library_handle = load_library(SAKE_LIBRARY_PATH);

	// break in debug mode after we have loaded the library
	if (debug)
	{
		debug_break();
	}

	// hook the random call if needed
	// NOTE: it is still not deterministic for some reason. needs to be checked
	hook_init();



	//server_client_test();
	socket_client_test();

	
	dlclose(library_handle);
	return 0;
}
