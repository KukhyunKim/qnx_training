/*
Server pseudo-code:
   • create a channel ( ChannelCreate() )
   • wait for a message ( MsgReceive() )
   – perform processing
   – reply ( MsgReply() )
Client pseudo-code:
   • attach to a channel ( ConnectAttach() )
   • send message ( MsgSend() )
*/

#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <process.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>

#include <gpio_devctl.h>
#include "gpio_client.h"
#include <errno.h>

#include <sys/dispatch.h> //name
#include "logger_utils.h"

int main ( int argc , char ** argv )
{
	struct _pulse msg;
	int reply;
	int coid = -1;
	int cnt = 0;
	pthread_setname_np (pthread_self ( ), "client");
	//pthread_setschedprio ( pthread_self ( ), 10 ); //thread priority
	//attach = name_attach(NULL, "ipc_server_name", 0);

	coid = name_open("ipc_server_name", 0);
	if (coid == -1) {
		printf ("can't open '%s'\n", "ipc_server_name");
		return 1;
	}

	for(cnt=0; cnt<10; cnt++) {
		reply = MsgSend(coid, &msg, sizeof(msg), NULL, 0);

		if (EOK != reply) {
			printf ("Failed to set power state; reply=%d\n", reply);
			return -1;
		} else {
			printf("ipc_client: MsgSend cnt:%d \n", cnt);
		}
		delay(500);
	}

	name_close(coid);

	return 0;
}


