#ifndef __SETH_OPTIONS_H__
#define __SETH_OPTIONS_H__

#include <stdint.h>

#include "siglent_dev.h"

#define MAX_EXTRA_ARGUMENTS_NUM 8

typedef struct seth_arguments {
    char* ipv4_address;
    uint16_t port;
    siglent_source_type_t source_type;
    int argc;
    char* argv[MAX_EXTRA_ARGUMENTS_NUM];
    char* scpi;
    // flags
    uint8_t custom_scpi : 1;
    uint8_t is_query : 1;
} seth_arguments_t;

void seth_arguments_dump(seth_arguments_t* args);
int parse_cmdline(int argc, char** argv, seth_arguments_t* args);

#endif // __SETH_OPTIONS_H__
