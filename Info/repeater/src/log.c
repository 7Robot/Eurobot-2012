#include <stdio.h> // *printf
#include <stdarg.h> // va_list
#include <time.h> // time(), localtime()
#include <errno.h> // errno
#include <string.h> // strerror()
#include <sys/types.h> // getpid()
#include <unistd.h> // getpid()

#include "log.h"

FILE* logfile;
char buffer[256];

int loginit(char* lgf)
{
	logfile = NULL;
	logfile = fopen(lgf, "a");
	if (logfile == NULL) {
		return -1;
	}
	return 0;
}

void lerror(int log, char* error)
{
	lprintf(log, "error: %s", strerror(errno));
}

void lprintf(int log, const char *format, ...)
{
	va_list va;
	time_t t;
	struct tm *date;
	
	if (logfile == NULL)
		return;
	
	fprintf(logfile, "(%d) ", getpid());
	
	time(&t);
	date = localtime(&t);
	fprintf(logfile, "[%d/%d %d:%d:%d] ",
			date->tm_mday, date->tm_mon,
			date->tm_hour, date->tm_min, date->tm_sec);
	
	switch (log) {
		case LOG_INFO:
			fprintf(logfile, "INFO");
			break;
		case LOG_NOTICE:
			fprintf(logfile, "NOTICE");
			break;
		case LOG_WARNING:
			fprintf(logfile, "WARNING");
			break;
		case LOG_ERR:
			fprintf(logfile, "ERROR");
			break;
	}
	fprintf(logfile, "\t");
	
	va_start(va, format);
	vfprintf(logfile, format, va);
	va_end(va);
	
	fflush(logfile);
} 
