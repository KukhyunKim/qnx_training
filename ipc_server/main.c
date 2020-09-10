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

#define LOG(sev,__fmt,...) logger_log (1,1,sev, __fmt, ##__VA_ARGS__)

int main ( int argc , char ** argv )
{
	name_attach_t *attach;
	int rcvid;
	int cnt=0;
	struct _pulse msg;
	pthread_setname_np (pthread_self ( ), "server");
	pthread_setschedprio ( pthread_self ( ), 8 ); //thread priority

	attach = name_attach(NULL, "ipc_server_name", _NTO_CHF_FIXED_PRIORITY);
	if (attach == NULL) {
		LOG(QCLOG_ERROR, "Cannot create attach point");
		return -1;
	}

	while (1)
	{
		printf("ipc_server: wait msg...\n");
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == -1)
			LOG(QCLOG_ERROR, "Failed to receive pulse with error: %s\n",
					strerror(errno));
		printf("ipc_server: MsgReceive. cnt:%d\n", cnt);
		cnt++;

		MsgReply(rcvid, EOK, NULL, 0);
		printf("ipc_server: MsgReply\n");

	}

	return 0;
}


