
#define LOG_INFO 1
#define LOG_NOTICE 2
#define LOG_WARNING 3
#define LOG_ERROR 4

char* lgbf;

void servlog(int type);
void lerror(int type, char* error);
int install_sig_handler(int signo, int flags, struct sigaction* oldsate, void(*sig_handler)(int));

