#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFMAX 4096

long where_jpeg_endofimage(unsigned char *buf, long size)
{
	long p;
	int stage = 0;

	for(p = size-1; p>0l; p--){
		switch(stage){
		case 0:
			if(buf[p] == 0xd9){
				stage++;
			}
			break;
		case 1:
			if(buf[p] == 0xff){
				stage++;
			} else {
				stage--;
			}
			break;
		case 2:
			return p + 3;
			break;
		default:
			fprintf(stderr, "unsupported stage: %d\n", stage);
			break;
		}
	}
	return 0;
}


size_t getfilesize(int fd)
{
	struct stat sb;

	assert(fstat(fd, &sb) >= 0);
	return sb.st_size;
}


int jpeg_trancate(char *ifname)
{
	int ifd;
	void *ifmap;
	long ifsize;
	long offset;

	ifd = open(ifname, O_RDWR);
	assert(ifd != -1);
	ifsize = getfilesize(ifd);	/* get file size */

	fprintf(stderr, "open %s [%ld]", ifname, ifsize);

	ifmap = mmap(NULL, ifsize, PROT_READ, MAP_PRIVATE, ifd, 0);
	assert(ifmap != MAP_FAILED);

	offset = where_jpeg_endofimage(ifmap, ifsize);

	fprintf(stderr, " -> [%ld = 0x%x] (%ld)\n", offset, offset, offset-ifsize);

	ftruncate(ifd, offset);
	fprintf(stderr, "truncated\n");
	return offset;
}


int main(int argc, char **argv)
{
	int i;
	
	if(argc < 2){
		fprintf(stderr, "please specify file name\n");
		return -1;
	}

	for(i = 1; i < argc; i++){
		jpeg_trancate(argv[i]);
	}
	return 0;
}

