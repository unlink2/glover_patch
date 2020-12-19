#include <stdio.h>
#include <ftdi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <editline.h>

#define FILE_CHUNK 0x8000
#define USB_VENDOR 0x0403
#define USB_DEVICE 0x6001
#define USB_READ_TIMEOUT 5000
#define USB_WRITE_TIMEOUT 5000
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define BUFFER_SIZE 1024

int stop = 0;
int listen = 0;

void inthand(int signum) {
    // first disable listening mode
    if (listen) {
        listen = 0;
        return;
    }
    stop = 1;
}

void clear_buffer(unsigned char *recv_buff, unsigned char *send_buff) {
    // init buffer
    memset(send_buff, 0, BUFFER_SIZE);
    memset(recv_buff, 0, BUFFER_SIZE);
}

struct ftdi_context* init() {
    // int baudrate = 115200;
    int ret;
    struct ftdi_context* ftdi;
    if ((ftdi = ftdi_new()) == 0) {
        fprintf(stderr, "ftdi_new failed\n");
        return NULL;
    }

    if ((ret = ftdi_usb_open(ftdi, USB_VENDOR, USB_DEVICE)) < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret,
                ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return NULL;
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
    return ftdi;
}

void cleanup(struct ftdi_context *ftdi) {
    int ret;

    if ((ret = ftdi_usb_close(ftdi)) < 0) {
        fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret,
                ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        exit(EXIT_FAILURE);
        return;
    }

    ftdi_free(ftdi);
}

void clear_screen() {
    fprintf(stderr, "\e[1;1H\e[2J");
}

void send_batch(struct ftdi_context *ftdi, unsigned char *send_buff, unsigned char *recv_buff, char *file) {
    int ret_r;

    FILE *pf;
    pf = fopen(file, "r");
    if (!pf) {
        return;
    }

    size_t read;
    size_t len;
    char *line = NULL;
    while ((read = getline(&line, &len, pf)) != -1) {
        ret_r = 0;
        clear_buffer(send_buff, recv_buff);
        line[strlen((char*)line)-1] = '\0'; // remove new line char

        fprintf(stderr, ">> %s\n", line);
        strcpy((char*)send_buff, line);

        ftdi_write_data(ftdi, send_buff, BUFFER_SIZE);
        usleep(100000); // wait for response
        ret_r = ftdi_read_data(ftdi, recv_buff, BUFFER_SIZE);
        if (ret_r == 0) {
            return;
        } else {
            fprintf(stderr, ">> %s\n", recv_buff);
        }
    }
    if (line) {
        free(line);
    }
}

void mainloop(struct ftdi_context *ftdi, unsigned char *send_buff, unsigned char *recv_buff) {
    int ret_r;
    while (!stop) {
        clear_buffer(send_buff, recv_buff);
        ret_r = 0;

        // if in listening mode do not allow input, just receive
        // CTRL+C will first exit listen mode before exiting the loop
        if (!listen) {
            // fprintf(stderr, ">> ");
            // send cmdt to OS to test usb communication
            // fgets((char*)send_buff, BUFFER_SIZE, stdin);
            // send_buff[strlen((char*)send_buff)-1] = '\0'; // remove new line char
            char *buffer = readline(">> ");
            add_history(buffer);
            strcpy((char*)send_buff, buffer);
            free(buffer);
        } else {
            strcpy((char*)send_buff, "listen");
            // clear_screen();
        }

        // exit commnad
        if (strcmp((char*)send_buff, "exit") == 0) {
            stop = 1;
            continue;
        } else if (strcmp((char*)send_buff, "listen") == 0) {
            listen = 1;
        }

        ftdi_write_data(ftdi, send_buff, BUFFER_SIZE);
        usleep(100000); // wait for response
        ret_r = ftdi_read_data(ftdi, recv_buff, BUFFER_SIZE);

        if (ret_r > 0) {
            int chidx;
            fprintf(stderr, "<< ");
            for (chidx = 0; chidx < BUFFER_SIZE; ++chidx) {
                if (recv_buff[chidx] == '\0') {
                    break;
                }
                fprintf(stderr, "%c", recv_buff[chidx]);
            }
            fprintf(stderr, "\n");
        } else {
            fprintf(stderr, "no response\n");
        }

        usleep(500);
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, inthand);

    // usb buffer
    unsigned char send_buff[BUFFER_SIZE];
    unsigned char recv_buff[BUFFER_SIZE];

    struct ftdi_context *ftdi = init();
    if (!ftdi) {
        return EXIT_FAILURE;
    }

    clear_buffer(send_buff, recv_buff);

    // send batch commands
    for (int i = 1; i < argc; i++) {
        send_batch(ftdi, send_buff, recv_buff, argv[i]);
    }

    mainloop(ftdi, send_buff, recv_buff);

    cleanup(ftdi);
    clear_buffer(send_buff, recv_buff);

    return 0;
}
