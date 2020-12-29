#include "options.h"

#include <argp.h>
#include <stdlib.h>

#include "seth.h"

//#define DEBUG

const char *argp_program_version = "seth " SETH_VERSION;
const char *argp_program_bug_address = "<di0x7c5@gmail.com>";

char doc[] = "Remote control Siglent DSO using ethernet";

struct argp_option options[] = {
    { "address", 's', "IP", 0, "IPv4 address of device", 0 },
    { "port", 'p', "PORT", 0, "Port number (default 5025)", 0},
//    { "source", 'c', "[C1,C2,C3,C4]", 0, "Channel number to which the command applies (default 1)", 0},
    { NULL, 'x', "\"COMMAND\"", 0, "Send custom SCPI command" },
    { NULL, 'q', "\"QUERY\"", 0, "Send custom SCPI query"},
    { 0 }
};

error_t argp_parse_opt(int key, char *arg, struct argp_state *state) {
    seth_arguments_t *arguments = (seth_arguments_t*)state->input;

    switch (key) {
        case 's':
            arguments->ipv4_address = arg;
            break;
        case 'p':
            arguments->port = atoi(arg);
            break;
        case 'c':
            arguments->source_type = source_type_from_str(arg);
            break;
        case 'q':
            arguments->is_query = 1;
            // Intentionally no break
        case 'x':
            arguments->custom_scpi = 1;
            arguments->scpi = arg;
            break;
        case ARGP_KEY_ARG:
             if (state->arg_num >= MAX_EXTRA_ARGUMENTS_NUM)
                return ARGP_ERR_UNKNOWN;
             arguments->argc++;
             arguments->argv[state->arg_num] = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

struct argp argp = { options, argp_parse_opt, NULL, doc, 0, 0, 0 };

int parse_cmdline(int argc, char** argv, seth_arguments_t* args) {
    int rc;

    args->ipv4_address = NULL;
    args->port = SIGLENT_PORT_DEFAULT;
    args->source_type = SOURCE_UNKNOWN;
    args->argc = 0;

    rc = argp_parse(&argp, argc, argv, 0, 0, &args);

#ifdef DEBUG
    seth_arguments_dump(&args);
#endif
    return rc;
}

void seth_arguments_dump(seth_arguments_t* args) {
    LOG_INFO("--------------------------------");
    LOG_INFO("ipv4_address: %s", args->ipv4_address);
    LOG_INFO("port: %u", args->port);
    LOG_INFO("source_type: %d", args->source_type);
    LOG_INFO("argc: %d", args->argc);

    for (int i=0; i<MAX_EXTRA_ARGUMENTS_NUM; ++i)
        LOG_INFO("argv[%d]: %s", i, (args->argv[i] ? args->argv[i] : ""));

    LOG_INFO("command_raw: %s", (args->custom_scpi ? "true" : "false"));
    LOG_INFO("custom_command: %s", args->scpi);
    LOG_INFO("--------------------------------");
}
