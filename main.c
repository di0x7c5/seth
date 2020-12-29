#include <string.h>
#include <stdarg.h>

#include "seth.h"
#include "siglent_dev.h"
#include "options.h"

// TODO: Made it parametrized from argp (--silent, --verbose)
static seth_log_level_t seth_loglevel = LOGLEVEL_SILENT;
static const char loglevel_prefix[] = {
    'V', 'D', 'I', 'W', 'E', 'F'
};

void seth_log(const seth_log_level_t level, const char* msg, ...) {
    if (level < seth_loglevel)
        return;

    fprintf(stderr, "[%c] ", loglevel_prefix[level]);

    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    fprintf(stderr, "\r\n");
}

int main(int argc, char** argv) {
    siglent_dev_t dev;
    seth_arguments_t args;

    if (parse_cmdline(argc, argv, &args))
        return -SETH_PARSE_CMDLINE_ERROR;

    if (args.ipv4_address == NULL)
        return -SETH_NO_IPV4_ERROR;

    if (siglent_dev_init(&dev))
        return -SETH_DEV_INIT_ERROR;

    if (siglent_dev_connect(&dev, args.ipv4_address, args.port))
        return -SETH_DEV_CONNECT_ERROR;

    // Custom SCPI command or query
    if (args.custom_scpi) {
        LOG_DEBUG("Custom SCPI flag set.");

        if (args.scpi != NULL && strlen(args.scpi) > 0) {
            siglent_dev_execute(&dev, args.scpi,
                    (args.is_query || strchr(args.scpi, '?')));
        } else {
            LOG_WARNING("No SCPI command specified, there is no spoon?");
        }
    } else {
        LOG_DEBUG("...");
    }

    if (siglent_dev_disconnect(&dev))
        return -SETH_DEV_DISCONNECT_ERROR;
    
    if (siglent_dev_deinit(&dev))
        return -SETH_DEV_DEINIT_ERROR;

    return SETH_NO_ERROR;
}
