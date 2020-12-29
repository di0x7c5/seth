#ifndef _SETH_H_
#define _SETH_H_

#include <stdio.h>

#define SETH_VERSION "0.1"
#define SIGLENT_PORT_DEFAULT 5025

typedef enum seth_error {
    SETH_NO_ERROR = 0,
    SETH_PARSE_CMDLINE_ERROR,
    SETH_NO_IPV4_ERROR,
    SETH_DEV_INIT_ERROR,
    SETH_DEV_CONNECT_ERROR,
    SETH_DEV_DISCONNECT_ERROR,
    SETH_DEV_DEINIT_ERROR
} seth_error_t;

typedef enum seth_log_level {
    LOGLEVEL_VERBOSE,
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL,
    LOGLEVEL_SILENT
} seth_log_level_t;

#define LOG_VERBOSE(msg, arg...) seth_log(LOGLEVEL_VERBOSE, msg, ##arg)
#define LOG_DEBUG(msg, arg...)   seth_log(LOGLEVEL_DEBUG, msg, ##arg)
#define LOG_INFO(msg, arg...)    seth_log(LOGLEVEL_INFO, msg, ##arg)
#define LOG_WARNING(msg, arg...) seth_log(LOGLEVEL_WARNING, msg, ##arg)
#define LOG_ERROR(msg, arg...)   seth_log(LOGLEVEL_ERROR, msg, ##arg)
#define LOG_FATAL(msg, arg...)   seth_log(LOGLEVEL_FATAL, msg, ##arg)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*x))

void seth_log(const seth_log_level_t level, const char* msg, ...);

#endif // _SETH_H_
