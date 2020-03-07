#include <stdio.h>
#include <ftdi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_CHUNK 0x8000
#define USB_VENDOR 0x0403
#define USB_DEVICE 0x6001
#define USB_READ_TIMEOUT 5000
#define USB_WRITE_TIMEOUT 5000
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define BUFFER_SIZE 512

int stop = 0;

void inthand(int signum) {
      stop = 1;
}

int main(int argc, char **argv) {
    signal(SIGINT, inthand);

    // usb buffer
    unsigned char send_buff[BUFFER_SIZE];
    unsigned char recv_buff[BUFFER_SIZE];

    // int baudrate = 115200;

    int ret;
    int ret_r;
    struct ftdi_context* ftdi;
    if ((ftdi = ftdi_new()) == 0) {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    if ((ret = ftdi_usb_open(ftdi, USB_VENDOR, USB_DEVICE)) < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret,
                ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    } else {
        // read/write timeout e.g. 500ms
        ftdi->usb_read_timeout = USB_READ_TIMEOUT;
        ftdi->usb_write_timeout = USB_WRITE_TIMEOUT;
    }

    /* ret = ftdi_set_baudrate(ftdi, baudrate);
    if (ret < 0) {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        exit(-1);
    } */

    if (ftdi->type == TYPE_R) {
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);
    }

    // init buffer
    memset(send_buff, 0, BUFFER_SIZE);
    memset(recv_buff, 0, BUFFER_SIZE);

    while (!stop) {
        memset(send_buff, 0, BUFFER_SIZE);
        memset(recv_buff, 0, BUFFER_SIZE);
        ret_r = 0;

        printf(">> ");
        // send cmt to OS to test usb communication
        fgets((char*)send_buff, BUFFER_SIZE, stdin);
        ftdi_write_data(ftdi, send_buff, BUFFER_SIZE);

        ret_r = ftdi_read_data(ftdi, recv_buff, BUFFER_SIZE);

        if (ret_r > 0) {
            int chIdx;
            printf("<< ");
            for (chIdx = 0; chIdx < BUFFER_SIZE; ++chIdx) {
                if (recv_buff[chIdx] == '\0') {
                    break;
                }
                printf("%c", recv_buff[chIdx]);
            }
        }

        usleep(500);
    }

    // clear buffer
    memset(send_buff, 0, BUFFER_SIZE);
    memset(recv_buff, 0, BUFFER_SIZE);
    ret_r = 0;

    if ((ret = ftdi_usb_close(ftdi)) < 0) {
        fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret,
                ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    }

    ftdi_free(ftdi);

    return 0;
}
