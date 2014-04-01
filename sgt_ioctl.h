#ifndef SGT_IOCTL_H
#define SGT_IOCTL_H

struct sgt_buffer {
	uint64_t address;
	uint64_t size;
};

#define SGT_GET_MAX_SIZE	_IOW('Z', 80, struct sgt_buffer)
#define SGT_RESERVE		_IOWR('Z', 81, struct sgt_buffer)
#define SGT_UNRESERVE		_IOR('Z', 82, struct sgt_buffer)
#define SGT_UNRESERVE_ALL	_IO('Z', 83)

#define SGT_DEBUGFS_DIR		"scatter-gather"
#define SGT_DEBUGFS_FILE	"sgt"

#endif
