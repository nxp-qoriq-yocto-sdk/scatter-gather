/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * This software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
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
	struct sgt_buffer buffer0, buffer1, buffer2, buffer3;
	int i;

	fd = open(ABS_FILE_PATH, O_RDONLY);
	if (fd < 0) {
		report_error("open");
		return -1;
	}

	/* Get available memory. */
	memset(&buffer0, 0x0, sizeof(buffer0));
	ret = ioctl(fd, SGT_GET_MAX_SIZE, &buffer0);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("Available memory: %lluMB\n", buffer0.size / (1024 * 1024));

	/* Simulate default buffer for satrace */
	for (i = 0; i < 100; i++) {
		memset(&buffer1, 0x0, sizeof(buffer1));
		buffer1.size = 0x4000;
		ret = ioctl(fd, SGT_RESERVE, &buffer1);
		if (ret < 0) {
			report_error("ioctl");
			return -1;
		}

		ret = ioctl(fd, SGT_UNRESERVE_ALL);
		if (ret < 0) {
			report_error("ioctl");
			return -1;
		}
	}


	/* Allocate small buffer - 50KB. */
	memset(&buffer1, 0x0, sizeof(buffer1));
	buffer1.size = 50 * KB;
	ret = ioctl(fd, SGT_RESERVE, &buffer1);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("First table address: %llx\n", buffer1.address);

	/* Allocate medium buffer - 20MB. */
	memset(&buffer2, 0x0, sizeof(buffer2));
	buffer2.size = 20 * MB;
	ret = ioctl(fd, SGT_RESERVE, &buffer2);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("Second table address: %llx\n", buffer2.address);

	/* Allocate large buffer - 400MB. */
	memset(&buffer3, 0x0, sizeof(buffer3));
	buffer3.size = 400 * MB;
	ret = ioctl(fd, SGT_RESERVE, &buffer3);
	if (ret < 0) {
		report_error("ioctl");
		return -1;
	}
	printf("Third table address: %llx\n", buffer3.address);

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
