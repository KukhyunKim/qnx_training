#include <stdio.h>
#include <sys/neutrino.h>
#include <pthread.h>

int chid = -1;

static void* ipc_server (void)
{
	int rc;
    struct _pulse msg;

	printf("-k ipc_server \n");

    chid = ChannelCreate (0);
    if ( chid == -1 ) {
        rc = errno ;
        goto out;
	}
	printf("-k ipc_server: ChannelCreate done. chid:%d\n", chid);

	rc = -1;
	while(rc < 0)
	{
		printf("-k ipc_server: wait msg...\n");
		rc = MsgReceive (chid, &msg, sizeof(struct _pulse), NULL);
		printf("-k ipc_server: MsgReceive rc:%d\n", rc);

		MsgReply (rc, EOK, &msg, sizeof(msg));
		printf("-k ipc_server: MsgReply\n");
	}

out:
	ChannelDestroy ( chid ) ;
	printf("-k ipc_server: ChannelDestroy\n");
    pthread_exit ( (void*)rc ) ;
}

static void* ipc_client (void)
{
	int rc, coid;
    struct _pulse snd_msg;
	int rev_msg;

	printf("-k ipc_client\n");

	while ( chid < 0 ) {
		printf("-k ipc_client: wating. chid not create!\n");
	}

    coid = ConnectAttach (0, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if ( coid == -1 ) {
		printf("-k ipc_client: file ConnectAttach, coid:%d\n", coid);
        goto out;
	}
	printf("-k ipc_client: ConnectAttach, coid:%d\n", coid);

	rc = MsgSend (coid, &snd_msg, sizeof(snd_msg), &rev_msg, sizeof(rev_msg));
	printf("-k ipc_client: MsgSend\n");

out:
	ConnectDetach ( coid );
	printf("-k ipc_client: ConnectDetach\n");
    pthread_exit ( (void*)rc ) ;
}


int main ( int argc , char ** argv )
{
	int rc;

#if 0
	// thread
	printf("\t-k ipc_test thread!\n");
    pthread_t tid, tid1;
    pthread_create ( &tid, NULL , ipc_server, NULL);
    pthread_create ( &tid1, NULL , ipc_client, NULL);

    pthread_join (tid, (void*)&rc);
	printf("-k ipc_server: end.\n");
    pthread_join (tid1, (void*)&rc);
	printf("-k ipc_client: end.\n");
#else
	// fork
	printf("\n\t-k ipc_test fork!\n");

	printf("-p ipc_server \n");

    chid = ChannelCreate (0);
    if ( chid == -1 ) {
        rc = errno ;
        goto out;
	}
	printf("-p ipc_server: ChannelCreate done. chid:%d\n", chid);

    pid_t pid;
    pid_t get_pid;

	get_pid = getpid();
	printf("get_pid (parent!!) :%d\n", get_pid);

    if ( -1 == ( pid = fork () )) {
        printf("fork failed");
        rc = -1;
        goto out;
    }

    if ( pid == 0 ) { //child
		printf("fork return 0 to child!!:%d\n", pid);
		//ipc_client();

		int coid;
		struct _pulse snd_msg;
		int rev_msg;
		printf("-c ipc_client\n");

		while ( chid < 0 ) {
			printf("-c ipc_client: wating. chid not create!\n");
		}

		coid = ConnectAttach (0, get_pid, chid, _NTO_SIDE_CHANNEL, 0);
		if ( coid == -1 ) {
			printf("-c ipc_client: file ConnectAttach, chid:%d, coid:%d\n", chid, coid);
			goto out;
		}
		printf("-c ipc_client: ConnectAttach, coid:%d\n", coid);

		rc = MsgSend (coid, &snd_msg, sizeof(snd_msg), &rev_msg, sizeof(rev_msg));
		printf("-c ipc_client: MsgSend\n");
	}
	else { //parent
		printf("child pid !!:%d\n", pid);
		//ipc_server();

		struct _pulse msg;
		rc = -1;
		while(rc < 0)
		{
			printf("-p ipc_server: wait msg...\n");
			rc = MsgReceive (chid, &msg, sizeof(struct _pulse), NULL);
			printf("-p ipc_server: MsgReceive rc:%d\n", rc);

			MsgReply (rc, EOK, &msg, sizeof(msg));
			printf("-p ipc_server: MsgReply\n");
		}

		wait();
	}
#endif

	return 0;

out:
	printf("-k main out! rc:%d\n", rc);

	return -1;

}
