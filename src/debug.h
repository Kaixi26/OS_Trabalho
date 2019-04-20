#ifndef __SODEBUG_H__
#define __SODEBUG_H__

#include <unistd.h>
#include <fcntl.h>

#define REPORT_ERROR(str) write (STDERR_FILENO, str, strlen (str))

#ifdef VERBOSE_2
#define VERBOSE_1
#define REPORT_ERROR_V2(str) REPORT_ERROR(str)
#else
#define REPORT_ERROR_V2(str)
#endif

#ifdef VERBOSE_1
#define REPORT_ERROR_V1(str) REPORT_ERROR(str)
#else
#define REPORT_ERROR_V1(str)
#endif

#define REP_ERR_GOTO_V1(str, label) {REPORT_ERROR_V1(str); goto label;}
#define REP_ERR_GOTO_V2(str, label) {REPORT_ERROR_V2(str); goto label;}


#endif
