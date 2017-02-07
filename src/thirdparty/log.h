#ifndef _LOG_H_
#define _LOG_H_

void log_info(const char *txt, ...);
void log_warning(const char *txt, ...);
void log_error(const char *txt, ...);
void log_exit(const char *txt, ...);
void log_init(const char* logpath);

#endif
