#ifndef __SIGLENT_DEV_H__
#define __SIGLENT_DEV_H__

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DEV_TX_BUFFER_SIZE 64
#define DEV_RX_BUFFER_SIZE 512

typedef struct siglent_dev {
    int fd;
    struct sockaddr_in address;
    // TODO: TX buffer can be allocated during send, not needed here
    size_t tx_size;
    unsigned char* tx;
} siglent_dev_t;

typedef enum siglent_source_type {
    SOURCE_UNKNOWN,
    SOURCE_C1,
    SOURCE_C2,
    SOURCE_C3,
    SOURCE_C4,
    SOURCE_MATH
} siglent_source_type_t;

int siglent_dev_init(siglent_dev_t* dev);
int siglent_dev_deinit(siglent_dev_t* dev);
int siglent_dev_connect(siglent_dev_t* dev, const char* addr, uint16_t port);
int siglent_dev_disconnect(siglent_dev_t* dev);
int siglent_dev_send(siglent_dev_t* dev, const char* cmd);
int siglent_dev_recv(siglent_dev_t* dev, unsigned char** data);
int siglent_dev_execute(siglent_dev_t *dev, const char* cmd, uint8_t is_querry);

siglent_source_type_t source_type_from_str(char* arg);
const char* source_type_name(siglent_source_type_t type);

#endif // __SIGLENT_DEV_H__
