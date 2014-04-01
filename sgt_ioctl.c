#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sgt_ioctl.h"

#define DEBUGFS_PATH	"/sys/kernel/debug"
#define ABS_FILE_PATH	DEBUGFS_PATH "/" SGT_DEBUGFS_DIR "/" SGT_DEBUGFS_FILE

#define KB		1024
#define MB		(1024 * KB)

#define str(s)		#s
#define xstr(s)		str(s)
#define report_error(s)	perror(__FILE__ ":" xstr(__LINE__) ": " s)

/* Tutorial on how user space can access the module via ioctl calls. */
int main(void)
{
	int fd;
	int ret;
	struct sgt_buffer buffer0, buffer1, buffer2;

	fd = open(ABS_FILE_PATH, O_RDONLY);
	if (fd < 0) {
		report_error("open");
		return -1;
	}

	/* Get available memory. */
	ret = ioctl(fd, SGT_GET_MAX_SIZE, &buffer0);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("Available memory: %lluMB\n", buffer0.size / (1024 * 1024));

	/* Allocate small buffer - 50KB. */
	buffer1.size = 50 * KB;
	ret = ioctl(fd, SGT_RESERVE, &buffer1);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("First table address: %llx\n", buffer1.address);

	/* Allocate large buffer - 400MB. */
	buffer2.size = 400 * MB;
	ret = ioctl(fd, SGT_RESERVE, &buffer2);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("Second table address: %llx\n", buffer2.address);

	/* Deallocate one of the buffers. */
	ret = ioctl(fd, SGT_UNRESERVE, &buffer2);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}

	/* Deallocate the remaining buffers. */
	ret = ioctl(fd, SGT_UNRESERVE_ALL);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}

	return 0;
}
