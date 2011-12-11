
#define LOG_INFO 1
#define LOG_NOTICE 2
#define LOG_WARNING 3
#define LOG_ERR 4

int loginit(char* logfile);
void lprintf(int log, const char *format, ...);
void lerror(int log, char* error);
