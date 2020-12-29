#include "siglent_dev.h"
#include "seth.h"

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int siglent_dev_init(siglent_dev_t* dev) {
    assert(dev);

    dev->tx_size = DEV_TX_BUFFER_SIZE;
    dev->tx = (unsigned char*) malloc(dev->tx_size);

    if (dev->tx == NULL) {
        LOG_ERROR("Cannot allocate device buffer for TX");
        goto init_error;
    }

    if (dev->tx == NULL) {
        LOG_ERROR("Cannot allocate device buffer for RX");
        goto init_error;
    }

    dev->fd = socket(PF_INET, SOCK_STREAM, 0);

    if (dev->fd == -1) {
        LOG_ERROR("Socket creation failed, err=%d", dev->fd);
        goto init_error;
    }

    memset(dev->tx, 0, dev->tx_size);
    memset(&dev->address, 0, sizeof(struct sockaddr_in));

    return 0;

init_error:
    if (dev->tx) {
        free(dev->tx);
        dev->tx = NULL;
    }

    return -1;
}

int siglent_dev_deinit(siglent_dev_t* dev) {
    assert(dev);

    free(dev->tx);
    dev->tx = NULL;

    return 0;
}

int siglent_dev_connect(siglent_dev_t* dev, const char* addr, uint16_t port) {
    int rc;

    assert(dev != NULL);
    assert(addr != NULL);

    dev->address.sin_family = PF_INET;
    dev->address.sin_port = htons(port);
    dev->address.sin_addr.s_addr = inet_addr(addr);
    // ^ use inet_ntoa to get IP string

    rc = connect(dev->fd, (struct sockaddr*)&dev->address,
            sizeof(struct sockaddr_in));

    if (rc == -1) {
        LOG_ERROR("Cannot connect to %s:%d, rc=%u", addr, port, rc);
        return rc;
    }

    LOG_INFO("Connected to %s:%d", addr, port);
    return 0;
}

int siglent_dev_disconnect(siglent_dev_t* dev) {
    int rc;

    assert(dev);

    rc = close(dev->fd);

    if (rc == -1) {
        LOG_ERROR("Close socket failed, rc=%d", rc);
        return rc;
    }

    LOG_INFO("Disconnected from %s:%d",
            inet_ntoa(dev->address.sin_addr),
            ntohs(dev->address.sin_port));
    return 0;
}

int siglent_dev_send(siglent_dev_t* dev, const char* cmd) {
    int rc;
    size_t cmdlen;

    assert(dev);
    assert(cmd);

    cmdlen = strlen(cmd);

    if ((cmdlen+1) > DEV_TX_BUFFER_SIZE) {
        LOG_ERROR("Command length exceed the TX buffer length limit.");
        return -1;
    }

    memset(dev->tx, 0, DEV_TX_BUFFER_SIZE);
    strncpy(dev->tx, cmd, cmdlen);
    dev->tx[cmdlen] = '\n';

    LOG_INFO("Sending: %s", cmd);
    rc = send(dev->fd, dev->tx, cmdlen+1, 0);

    if (rc < 0) {
        LOG_ERROR("Send failed, rc=%d", rc);
        // TODO: print errno
        return rc;
    }

    return rc;
}

int siglent_dev_recv(siglent_dev_t* dev, unsigned char** data) {
    int rc;
    unsigned char* rxbuff;
    size_t rxbuff_size;
    size_t rxbuff_received;

    assert(dev);
    assert(data);

    *data = NULL;
    rxbuff = NULL;
    rxbuff_size = 0;
    rxbuff_received = 0;

    do {
        rxbuff_size += DEV_RX_BUFFER_SIZE;
        rxbuff = realloc(rxbuff, rxbuff_size);

        if (rxbuff == NULL) {
            LOG_ERROR("Reallocation failed");
            return -1;
        }

        usleep(100); // Workaround for too quick recv for larger buffers.
        rc = recv(dev->fd, rxbuff + rxbuff_received, DEV_RX_BUFFER_SIZE, 0);

        if (rc < 0) {
            LOG_ERROR("Receive failed, rc=%d", rc);
            return rc;
        }

        rxbuff_received += rc;
    } while(rc == DEV_RX_BUFFER_SIZE);

    // Use only as much as You need ...
    rxbuff = realloc(rxbuff, rxbuff_received);
    *data = rxbuff;

    // Return the number of received bytes
    return rxbuff_received;
}

static const char* siglent_source_type_name[] = {
    "UNKNOWN", "C1", "C2", "C3", "C4", "MATH"
};

const char* source_type_name(siglent_source_type_t type) {
    return siglent_source_type_name[type];
}

siglent_source_type_t source_type_from_str(char* arg) {
    int i;

    for (i=0; i<ARRAY_SIZE(siglent_source_type_name); i++) {
        if (!strcmp(arg, siglent_source_type_name[i]))
            return i;
    }

    // Alternative names
    if (!strcmp(arg, "1")) return SOURCE_C1;
    if (!strcmp(arg, "2")) return SOURCE_C2;
    if (!strcmp(arg, "3")) return SOURCE_C3;
    if (!strcmp(arg, "4")) return SOURCE_C4;

    // NOTE: Consider to return SOURCE_UKNOWN
    LOG_WARNING("Cannot unroll source type from \"%s\". Using as default C1!", arg);
    return SOURCE_C1;
}

int siglent_dev_execute(siglent_dev_t *dev, const char* cmd, uint8_t is_querry) {
    int rc;
    
    assert(dev);
    assert(cmd);
    
    LOG_DEBUG("Executing SCPI %s: \"%s\"", is_querry ? "querry" : "command", cmd);

    rc = siglent_dev_send(dev, cmd);

    if (rc > 0 && is_querry) {
        unsigned char* data;

        LOG_DEBUG("SCPI is a query, trying to read response.");

        rc = siglent_dev_recv(dev, &data);
        if (rc > 0) {
            LOG_DEBUG("Received %d bytes from DSO, writing to STDOUT", rc);
            write(STDOUT_FILENO, data, rc);
        }

        free(data);
        data = NULL;
    }

    return 0;
}
