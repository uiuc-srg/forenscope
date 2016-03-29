#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int serial_fd;
char watch_ping_bytes[] = {0x02, 0xC0, 0xC8, 0x01, 0x01, 0x01};
char watch_set_timeout[] = {0x02, 0xC0, 0xC6, 0x01, /* timeout */ 0x05, 0x03};

void ping(void);
void read_resp(void);

int main(int argc, char*argv[])
{
	system("stty -F /dev/ttyS1 38400");

	// Open serial port
	serial_fd = open("/dev/ttyS1", O_RDWR | O_NONBLOCK);

	if(serial_fd == -1)
	{
		perror("Can't open serial port");
		fprintf(stderr, "Can't open serial port...\n");
		return -1;
	}

	write(serial_fd, watch_set_timeout, sizeof(watch_set_timeout));

	while(1)
	{
		fprintf(stderr, "Pinging watchdog...\n");
		ping();
		sleep(1);
		read_resp();
		sleep(20);
	}

	return 0;
}

void ping()
{
	write(serial_fd, watch_ping_bytes, sizeof(watch_ping_bytes));
}

void read_resp()
{
	char read_bytes[10];
	read(serial_fd, read_bytes, 3);
	fprintf(stderr, "Received response %x %x %x\n", read_bytes[0],
		read_bytes[1], read_bytes[2]);
}

