#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include <time.h>
#include <sys/wait.h>

#include "main.h"
#include "server.h"

#define OPEN_MAX 1024

FILE* logfile;
int* pids;
int process;
volatile sig_atomic_t run;

//~ struct services {
	//~ int* sockets;
	//~ int* pids;
	//~ int servicec;
//~ }


int write_pid(char* filename);
void quit_sig_handler(int signo);
void son_sig_handler(int signo);
void listener_sig_handler(int signo);
int install_sig_handler(int signo, int flags, struct sigaction* oldstate, void(*sig_handler)(int));
void lerror(int loglvl, char* error);


int main(int argc, char** argv)
{
	char* services[] = { "3520", "3521", "3522", "3523", "3524", "3525", "3526", "3527", "3528", "3529" };
	int servc = 10;
	int* sockets = NULL;
	char* options = "p:l:";
	int option;
	char* logfilename = NULL;
	char* pidfilename = NULL;
	mqd_t mq;
	struct mq_attr attr;
	char* buffer = NULL;
	unsigned int priority;
	int n;
	
	logfile = NULL;
	pids = NULL;
	process = 0;
	
	/* Daemonisation */
	chdir("/"); // Unblock mount point
	if (fork() != 0) {
		if ((mq = mq_open("/repeater", O_RDONLY|O_CREAT, 0600, NULL)) == (mqd_t)-1) {
			perror("mq_open");
			exit(EXIT_FAILURE);
		}
		if (mq_getattr(mq, &attr) != 0) {
			perror("mq_getattr");
			exit(EXIT_FAILURE);
		}
		if ((buffer = malloc(attr.mq_msgsize)) == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		if ((n=mq_receive(mq, buffer, attr.mq_msgsize, &priority)) < 0) { /* timedreceive ! */
			perror("mq_receive");
			exit(EXIT_FAILURE);
		}
		if (priority == 10) {
			fprintf(stderr, ">%s\n", buffer);
			free(buffer);
			mq_close(mq);
			mq_unlink("/repeater");
			exit(EXIT_FAILURE);
		} else {
			free(buffer);
			mq_close(mq);
			mq_unlink("/repeater");
			exit(EXIT_SUCCESS);
		}
	}
	setsid(); // Start new session
	if (fork() != 0) exit(EXIT_SUCCESS); // Fork in new session
	for (int i = 0; i < getdtablesize() ; i++) // Close all file descriptor
		close(i);
	
	if ((mq = mq_open("/repeater", O_WRONLY|O_CREAT, 0600, NULL)) == (mqd_t)-1) {
		perror("mq_open");
		exit(EXIT_FAILURE);
	}
	
	opterr = 0; /* No auto-error message */
	while((option = getopt(argc, argv, options)) != -1) {
		switch(option) {
			case 'p':
				pidfilename = optarg;
				break;
			case 'l':
				logfilename = optarg;
				break;
		}
	}
	
	if (pidfilename != NULL) write_pid(pidfilename);
	
	lgbf = malloc(4096 * sizeof(char));
	if (lgbf == NULL) {
		mq_send(mq, "malloc", strlen("malloc"), 10);
		mq_close(mq);
		//~ unlink(pidfilename);
		exit(EXIT_FAILURE);
	}
	if (logfilename == NULL)
		logfilename = "/var/log/repeteur.log";
	logfile = fopen(logfilename, "a");
	if (logfile == NULL) {
		sprintf(lgbf, "fopen(%s): %s", logfilename, strerror(errno));
		mq_send(mq, lgbf, strlen(lgbf), 10);
		mq_close(mq);
		//~ unlink(pidfilename);
		exit(EXIT_FAILURE);
	}
	mq_send(mq, "", strlen(""), 0);
	mq_close(mq);
	/* At this point, no more links with the father */
	
	if ((sockets = malloc(servc * sizeof(int))) == NULL) {
		lerror(LOG_ERROR, "malloc");
		//~ unlink(pidfilename);
		exit(EXIT_FAILURE);
	}
	if ((pids = malloc(servc * sizeof(int))) == NULL) {
		lerror(LOG_ERROR, "malloc");
		//~ unlink(pidfilename);
		exit(EXIT_FAILURE);
	}
	
	sprintf(lgbf, "Repeater server started");
	servlog(LOG_INFO);
	
	int i;
	for (i = 0; i < servc; i++) {
		if ((sockets[i] = create_tcp_server(NULL, services[i])) < 0) {
			sprintf(lgbf, "Can't create a server for service %s", services[i]);
			servlog(LOG_WARNING);
			continue;
		}
		int pid = fork();
		switch (pid) {
			case 0: // Son
				sprintf(lgbf, "(%d) Service %s launched", getpid(), services[i]);
				servlog(LOG_INFO);
				if(lisen_connection(sockets[i]) < 0) {
					sprintf(lgbf, "lisen_connection failed");
					servlog(LOG_ERROR);
					close(sockets[i]);
					exit(EXIT_FAILURE);
				}
				close(sockets[i]);
				exit(EXIT_SUCCESS);
				break;
			case -1:
				lerror(LOG_WARNING, "fork");
				close(sockets[i]);
				continue;
			default: // Father
				close(sockets[i]);
				pids[process++] = pid;
		}
				
	}
	
	/* accept is an exemple of interruptible function, that should not be restarted on SIGQUIT, SIGINT, … */
	install_sig_handler(SIGQUIT, 0, NULL, quit_sig_handler);
	install_sig_handler(SIGINT, 0, NULL, quit_sig_handler);
	install_sig_handler(SIGTERM, 0, NULL, quit_sig_handler);
	install_sig_handler(SIGCHLD, SA_RESTART, NULL, son_sig_handler);
	
	sigset_t exitsigset;
	sigemptyset(&exitsigset);
	sigaddset(&exitsigset, SIGKILL);
	sigaddset(&exitsigset, SIGTERM);
	sigaddset(&exitsigset, SIGINT);
	
	if (process == 0) {
		sprintf(lgbf, "No services launched");
		servlog(LOG_ERROR);
		free(lgbf);
		fclose(logfile);
		//~ unlink(pidfilename);
		return EXIT_FAILURE;
	}
	
	run = 1;
	
	sigset_t oldsigset;
	sigprocmask(SIG_BLOCK, &exitsigset, &oldsigset);
	while (run)
		sigsuspend(&oldsigset);
	
	for (int i = 0; i < process; i++) {
		close(sockets[i]);
	}
	
	sprintf(lgbf, "Repeater server shutdown");
	servlog(LOG_INFO);
	
	free(lgbf);
	fclose(logfile);
	//~ unlink(pidfilename);
	return EXIT_SUCCESS;
}

void lerror(int loglvl, char* error)
{
	sprintf(lgbf, "error: %s", strerror(errno));
	servlog(loglvl);
}

void servlog(int type)
{
	time_t t;
	struct tm *date;
	
	time(&t);
	date = localtime(&t);
	
	fprintf(logfile, "(%d) [%d/%d %d:%d:%d] ", getpid(), date->tm_mday, date->tm_mon, date->tm_hour, date->tm_min, date->tm_sec);
	switch (type) {
		case LOG_INFO:
			fprintf(logfile, "INFO");
			break;
		case LOG_NOTICE:
			fprintf(logfile, "NOTICE");
			break;
		case LOG_WARNING:
			fprintf(logfile, "WARNING");
			break;
		case LOG_ERROR:
			fprintf(logfile, "ERROR");
			break;
	}
	fprintf(logfile, "\t%s\n", lgbf);
	fflush(logfile);
}

int write_pid(char* filename)
{
	FILE* pidf = fopen(filename, "w+");
	if (pidf == NULL) {
		fprintf(stderr, "fopen: Fail to open %s in w+ mode\n", filename);
		return -1;
	}
	fprintf(pidf, "%d\n", getpid());
	fclose(pidf);
	return 0;
}

void quit_sig_handler(int signo)
{
	signal(SIGCHLD, SIG_IGN); /* Ignore sons quit signal */
	for (int i = 0; i < process ; i++) {
		sprintf(lgbf, "Kill %i ...", pids[i]);
		servlog(LOG_INFO);
		kill(pids[i], SIGQUIT);
	}
	run = 0;
}

void son_sig_handler(int signo)
{
	sigset_t allsigmask, backupsigmask;
	sigfillset(&allsigmask); /* All signals */
	sigprocmask(SIG_SETMASK, &allsigmask, &backupsigmask);
	int status;
	int pid = wait(&status);
	for (int i = 0; i < process; i++) {
		if (pids[i] == pid) {
			pids[i] = pids[--process];
		}
	}
	sigprocmask(SIG_SETMASK, &backupsigmask, NULL);
	if (status != 0) {
		sprintf(lgbf, "Process %i quit with status %i", pid, status);
		servlog(LOG_NOTICE);
	}
	if (process == 0) {
		raise(SIGQUIT);
	}
}

int install_sig_handler(int signo, int flags, struct sigaction* oldsate, void(*sig_handler)(int))
{
	struct sigaction action;
	
	action.sa_handler = sig_handler; /* SIG_IGN: ignore, SIG_DFL: default action */
	sigemptyset(&(action.sa_mask)); /* no signal blocked during execution of signal handler */
	action.sa_flags = flags; /* SA_RESTART: restart interruptible function after, SA_RESETHAND: restore default after handler execution */
	
	if (sigaction(signo, &action, oldsate) != 0) { /* NULL : don't need to save old state */
		return -1;
	} else {
		return 0;
	}
}
