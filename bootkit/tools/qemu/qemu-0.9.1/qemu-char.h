#ifndef QEMU_CHAR_H
#define QEMU_CHAR_H

/* character device */

#define CHR_EVENT_BREAK 0 /* serial break char */
#define CHR_EVENT_FOCUS 1 /* focus to this terminal (modal input needed) */
#define CHR_EVENT_RESET 2 /* new connection established */


#define CHR_IOCTL_SERIAL_SET_PARAMS   1
typedef struct {
    int speed;
    int parity;
    int data_bits;
    int stop_bits;
} QEMUSerialSetParams;

#define CHR_IOCTL_SERIAL_SET_BREAK    2

#define CHR_IOCTL_PP_READ_DATA        3
#define CHR_IOCTL_PP_WRITE_DATA       4
#define CHR_IOCTL_PP_READ_CONTROL     5
#define CHR_IOCTL_PP_WRITE_CONTROL    6
#define CHR_IOCTL_PP_READ_STATUS      7
#define CHR_IOCTL_PP_EPP_READ_ADDR    8
#define CHR_IOCTL_PP_EPP_READ         9
#define CHR_IOCTL_PP_EPP_WRITE_ADDR  10
#define CHR_IOCTL_PP_EPP_WRITE       11

typedef void IOEventHandler(void *opaque, int event);

struct CharDriverState {
    int (*chr_write)(struct CharDriverState *s, const uint8_t *buf, int len);
    void (*chr_update_read_handler)(struct CharDriverState *s);
    int (*chr_ioctl)(struct CharDriverState *s, int cmd, void *arg);
    IOEventHandler *chr_event;
    IOCanRWHandler *chr_can_read;
    IOReadHandler *chr_read;
    void *handler_opaque;
    void (*chr_send_event)(struct CharDriverState *chr, int event);
    void (*chr_close)(struct CharDriverState *chr);
    void (*chr_accept_input)(struct CharDriverState *chr);
    void *opaque;
    int focus;
    QEMUBH *bh;
};

CharDriverState *qemu_chr_open(const char *filename);
void qemu_chr_close(CharDriverState *chr);
void qemu_chr_printf(CharDriverState *s, const char *fmt, ...);
int qemu_chr_write(CharDriverState *s, const uint8_t *buf, int len);
void qemu_chr_send_event(CharDriverState *s, int event);
void qemu_chr_add_handlers(CharDriverState *s,
                           IOCanRWHandler *fd_can_read,
                           IOReadHandler *fd_read,
                           IOEventHandler *fd_event,
                           void *opaque);
int qemu_chr_ioctl(CharDriverState *s, int cmd, void *arg);
void qemu_chr_reset(CharDriverState *s);
int qemu_chr_can_read(CharDriverState *s);
void qemu_chr_read(CharDriverState *s, uint8_t *buf, int len);
void qemu_chr_accept_input(CharDriverState *s);

/* async I/O support */

int qemu_set_fd_handler2(int fd,
                         IOCanRWHandler *fd_read_poll,
                         IOHandler *fd_read,
                         IOHandler *fd_write,
                         void *opaque);
int qemu_set_fd_handler(int fd,
                        IOHandler *fd_read,
                        IOHandler *fd_write,
                        void *opaque);

#endif
