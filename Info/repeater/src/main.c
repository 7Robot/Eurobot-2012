/** Repeater.
 * This program manages repeater' servers.
 */
 
 #include <stdlib.h> // exit()
 #include <stdio.h> // perror()
 #include <unistd.h> // getopt()
 
 #include "log.h"
 #include "daemon.h"
 
 int write_pid(char* pidfilename);
 
int main(int argc, char** argv)
{
	char* options = "p:l:";
	int option;
	char* logfilename = NULL;
	char* pidfilename = NULL;
	
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
	
	if (logfilename == NULL)
		logfilename = "/var/log/repeater.log";
	if (loginit(logfilename) < 0) {
		perror("loginit");
		exit(EXIT_FAILURE);
	}
	
	daemonize();
	
	if (pidfilename == NULL)
		pidfilename = "/var/run/repeater.pid";
	if (write_pid(pidfilename) < 0) {
		lerror(LOG_NOTICE, "write_pid");
	}
	
	
	//~ install_sig_handler(SIGQUIT, 0, NULL, quit_sig_handler);
	//~ install_sig_handler(SIGINT, 0, NULL, quit_sig_handler);
	//~ install_sig_handler(SIGTERM, 0, NULL, quit_sig_handler);
	//~ install_sig_handler(SIGCHLD, SA_RESTART, NULL, son_sig_handler);
	//~ 
	//~ sigset_t exitsigset;
	//~ sigemptyset(&exitsigset);
	//~ sigaddset(&exitsigset, SIGKILL);
	//~ sigaddset(&exitsigset, SIGTERM);
	//~ sigaddset(&exitsigset, SIGINT);
	
	reply_to_father("Ok \\o/", 0);
	lprintf(LOG_INFO, "Server started");
	
	//~ lprintf(LOG_INFO, "Salut %s\n", "toi");
	unlink(pidfilename);
	exit(EXIT_SUCCESS);
}

int write_pid(char* filename)
{
	FILE* pidf = NULL;
	if ((pidf = fopen(filename, "w+")) == NULL) {
		fprintf(stderr, "fopen: Fail to open %s in w+ mode\n", filename);
		return -1;
	}
	fprintf(pidf, "%d", getpid());
	fclose(pidf);
	return 0;
}
