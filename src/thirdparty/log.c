#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static FILE *logfile;

static void log_flush(void)
{
	fclose(logfile);
}

void log_init(const char* logpath)
{
	logfile = fopen(logpath, "w");
	if (!logfile)
		log_exit("Failed to initialize log.");
	atexit(&log_flush);
}

void log_info(const char *txt, ...)
{
	printf("INFO: ");
	va_list args;
	va_start(args, txt);
	vprintf(txt, args);
	if (logfile)
	{
		fprintf(logfile, "INFO: ");
		vfprintf(logfile, txt, args);
		fprintf(logfile, "\n");
	}
	va_end(args);
	printf("\n");
}

void log_warning(const char *txt, ...)
{
	printf("WARNING: ");
	va_list args;
	va_start(args, txt);
	vprintf(txt, args);
	if (logfile)
	{
		fprintf(logfile, "WARNING: ");
		vfprintf(logfile, txt, args);
		fprintf(logfile, "\n");
	}
	va_end(args);
	printf("\n");
}

void log_error(const char *txt, ...)
{
	printf("ERROR: ");
	va_list args;
	va_start(args, txt);
	vprintf(txt, args);
	if (logfile)
	{
		fprintf(logfile, "ERROR: ");
		vfprintf(logfile, txt, args);
		fprintf(logfile, "\n");
	}
	va_end(args);
	printf("\n");
}

void log_exit(const char *txt, ...)
{
	printf("FATAL ERROR: ");
	va_list args;
	va_start(args, txt);
	vprintf(txt, args);
	if (logfile)
	{
		fprintf(logfile, "FATAL ERROR: ");
		vfprintf(logfile, txt, args);
		fprintf(logfile, "\n");
	}
	va_end(args);
	printf("\n");
	exit(1);
}
