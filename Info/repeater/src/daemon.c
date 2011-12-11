#include <unistd.h> // chdir()
#include <stdio.h> // *printf()
#include <mqueue.h> // mq_*()
#include <stdlib.h> // exit()
#include <string.h> // strlen()

int wait_son_reply(char* reply);

char mqname[256];

void daemonize()
{
	int status;
	char* reply = NULL;
	
	sprintf(mqname, "/daemonize.%d", getpid());
	
	chdir("/"); // Unblock mount point
	if (fork() != 0) {
		status = wait_son_reply(reply);
		if (status < 0) {
			perror(reply);
			exit(EXIT_FAILURE);
		} else if (status == 0) {
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr, "%s\n", reply);
			exit(EXIT_FAILURE);
		}
	}
	setsid(); // Start new session
	if (fork() != 0) exit(EXIT_SUCCESS); // Fork in new session
	for (int i = 0; i < getdtablesize() ; i++) // Close all file descriptor
		close(i);
}

int wait_son_reply(char* reply)
{
	mqd_t mq;
	struct mq_attr attr;
	unsigned int status;
	
	if ((mq = mq_open(mqname, O_RDONLY|O_CREAT, 0600, NULL)) == (mqd_t)-1) {
		perror("mq_open");
		return -1;
	}
	if (mq_getattr(mq, &attr) != 0) {
		perror("mq_getattr");
		return -1;
	}
	if ((reply = realloc(reply, attr.mq_msgsize)) == NULL) {
		perror("realloc");
		return -1;
	}
	/* ↓ timedreceive is better ↓ */
	if (mq_receive(mq, reply, attr.mq_msgsize, &status) < 0) {
		perror("mq_receive");
		return -1;
	}
	mq_close(mq);
	mq_unlink(mqname);
	return status;
}

void reply_to_father(char* reply, int priority)
{
	mqd_t mq;
	if ((mq = mq_open(mqname, O_WRONLY|O_CREAT, 0600, NULL)) == (mqd_t)-1) {
		exit(EXIT_FAILURE);
	}
	mq_send(mq, reply, strlen(reply), priority);
	mq_close(mq);
}
