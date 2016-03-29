inline int rkm(int fd, unsigned long offset, void *buf, int size)
{
	if (lseek64(fd, (unsigned long long)offset, 0) == -1)
		return 0;
	if (read(fd, buf, size) != size)
		return 0;
	return size;
}

inline int wkm(int fd, unsigned long offset, void *buf, int size)
{
	if (lseek64(fd, (unsigned long long)offset, 0) == -1 )
		return 0;
	if (write(fd, buf, size) != size)
		return 0;
	return size;
}
