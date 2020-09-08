/*
// 2nd priority & interrupt - 20200908

// 1st ipc
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

int chid = -1;

static void interrupt_test(uint32_t gpio_number, uint32_t trigger, uint32_t irq_number)
{
	uint32_t irq;
	int interrupt_id;
	int status;
	int fd;
	struct sigevent int_event;

	if (-1 == (fd = gpio_open(NULL))) {
		printf("gpio_open() failed");
		goto exit_err;
	}

	if (irq_number == 0)
	{
		printf("gpio_set_interrupt_cfg()\n");
		if(-1 == gpio_set_interrupt_cfg(fd, gpio_number, trigger, NULL)) {
			printf("Failed to setup detect pin interrupt \n");
			goto exit_err;
		}

		printf("gpio_get_interrupt_cfg()\n");
		if(-1 == (status = gpio_get_interrupt_cfg(fd, gpio_number, &irq))) {
			printf("Failed to get irq corresponding to gpio %d \n", gpio_number);
			goto exit_err;
		}
		else {
			if (irq == -1) {
				printf("err. irq corresponding gpio:%d irq:%d status:%d -exiting\n", gpio_number, irq, status);
				goto exit_err;
			}
			printf(" irq corresponding to gpio:%d irq:%d status:%d\n",gpio_number, irq, status);
		}
	}

	// Attach Event ISR
	SIGEV_INTR_INIT(&int_event); //sigev_notify == SIGEV_INTR
	interrupt_id = InterruptAttachEvent(irq, &int_event, _NTO_INTR_FLAGS_TRK_MSK);

	InterruptUnmask(irq, interrupt_id); //en HW intr
	printf("Wakeup setup successfully completed  and now wait for interrupt \n");

	// ... Wait for interrupt
	status = InterruptWait_r(0, NULL );

	// determine the source of the interrupt

	if (status != EOK) {
		printf("InterruptWait_r failed with error %d \n",status);
	}
	else {
		printf("Got Interrupt :SUCCESS \n");
		InterruptMask(irq, interrupt_id); //disable HW intr
	}

	InterruptDetach(interrupt_id);

exit_err:
	return;
}


int main ( int argc , char ** argv )
{
	int rc=0;
	int cnt=0;
	pid_t pid;
	pid_t get_pid;
	pid_t c_pid;
	struct sched_param param;

	printf("\n\tTOP: 2nd:priority & interrupt / 1st:ipc_test fork!\n\n");

	// 2nd. interrupt
	printf("\n\t2.interrupt test\n\n");

	//GPIO 86 : uart; SOC_DBG_UART_RX_S4A_1P8; default PULL UP
	interrupt_test(86, 2, 0); //1:tri_high, 2:low 3:rising 4:falling


	// 1st. ipc_test
	printf("\n\t1.ipc test & 2.priority test\n\n");
	printf("-p ipc_server \n");
    chid = ChannelCreate (0);
    if ( chid == -1 ) {
        rc = errno ;
        goto out;
	}
	printf("-p ipc_server: ChannelCreate done. chid:%d\n", chid);

	get_pid = getpid();
	printf("-p get_pid:%d\n", get_pid);

	sched_getparam(get_pid, &param); //Get the current scheduling parameters of a process
	printf("The assigned priority is %d.\n", param.sched_priority);
	printf("The current priority is %d.\n", param.sched_curpriority);

	// fork
    if ( -1 == ( pid = fork () )) {
        printf("fork failed");
        rc = -1;
        goto out;
    }

    if ( pid == 0 ) { //child
		//ipc_client();
		int coid;
		struct _pulse snd_msg;
		int rev_msg;
		printf("-c ipc_client\n");

		c_pid = getpid();
		printf("-c c_pid:%d\n", c_pid);

		sched_getparam(c_pid, &param);
		printf("The assigned priority is %d.\n", param.sched_priority);
		printf("The current priority is %d.\n", param.sched_curpriority);

		while ( chid < 0 ) {
			printf("-c ipc_client: wating. chid not create!\n");
		}

		coid = ConnectAttach (0, get_pid, chid, _NTO_SIDE_CHANNEL, 0);
		if ( coid == -1 ) {
			printf("-c ipc_client: file ConnectAttach, chid:%d, coid:%d\n", chid, coid);
			goto out;
		}
		printf("-c ipc_client: ConnectAttach, coid:%d\n", coid);

		for(int i=0; i < 10; i++)
		{
			rc = MsgSend (coid, &snd_msg, sizeof(snd_msg), &rev_msg, sizeof(rev_msg));
			printf("-c ipc_client: MsgSend. i:%d\n", i);

			delay(500); //500ms
		}
	}
	else { //parent
		//ipc_server();
		struct _pulse msg;

		while(cnt < 10)
		{
			printf("-p ipc_server: wait msg...\n");
			rc = MsgReceive (chid, &msg, sizeof(struct _pulse), NULL);
			printf("-p ipc_server: MsgReceive. cnt:%d\n", cnt++);

			MsgReply (rc, EOK, &msg, sizeof(msg));
			printf("-p ipc_server: MsgReply\n");

			if(cnt == 5) {
				printf("Change priority -pid:%d\n", get_pid);
				param.sched_priority = 8;
				sched_setparam(get_pid, &param);
				//nice(-3);

				sched_getparam(get_pid, &param); //Get the current scheduling parameters of a process
				printf("The assigned priority is %d.\n", param.sched_priority);
				printf("The current priority is %d.\n", param.sched_curpriority);
			}
		}

		wait(NULL); //Wait for the status of a terminated child process
		printf("-p ipc_server: wait for child to die. --done \n");
	}

	return 0;

out:
	printf("-k main out! rc:%d\n", rc);

	return -1;
}


