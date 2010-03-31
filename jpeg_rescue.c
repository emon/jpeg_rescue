#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXBUF 4096
#define BLOCKSIZE1 512		/* 1st block size */
#define BLOCKSIZE 4096		/* default block size */
#define MAXJPEGSIZE (1024*1024*10)


int
is_jpeg_startofimage(unsigned char *buf)
{
	if(buf[0] == 0xff &&
	   buf[1] == 0xd8){
		return 1;
	} else {
		return 0;
	}
}

		
int
main(int argc, char** argv)
{
	FILE *ifp;
	char *ifname;
	
	FILE *ofp = NULL;
	char ofname[8+1+3+1];
	size_t oflen = 0;

	unsigned char buf[MAXBUF];
	int cl = 0;
	int i = 0;

	if(argc != 2){
		fprintf(stderr, "please specify filename\n");
		return -1;
	}
	ifname = argv[1];

	if((ifp=fopen(ifname, "r"))==NULL){
		fprintf(stderr, "can not open file: '%s'\n", ifname);
		return -1;
	}

	for(cl = 0; !feof(ifp); cl++){ /* search first JPEG's SOI (start of image) */
		if(fread(buf, BLOCKSIZE1, 1, ifp)!=1){
			continue;
		}
		if(is_jpeg_startofimage(buf)){ /* find first JPEG */
			fseek(ifp, -BLOCKSIZE1, SEEK_CUR); /* rewind and break */
			fprintf(stderr, "find first SOI at cl=%d\n", cl);
			break;
		}
	}
	
	for(cl = 0; !feof(ifp); cl++){
		int err;

		if((err=fread(buf, BLOCKSIZE, 1, ifp))!=1){
			fprintf(stderr, "fread: error=%d, skip cl=%d\n", err, cl);
			continue;
		}
		if(oflen > MAXJPEGSIZE){
			fprintf(stderr, ", cut [%uz]\n", oflen);
			fclose(ofp);
			ofp = NULL;
			oflen = 0;
		}
		if(is_jpeg_startofimage(buf)){
			if(ofp!=NULL){
				fprintf(stderr, ", done [%uz]\n", oflen);
				fclose(ofp);
				ofp = NULL;
				oflen = 0;
			}
			snprintf(ofname, sizeof(ofname), "%08d.jpg", i++);
			fprintf(stderr, "writing: %s", ofname);
			oflen = 0;
			if((ofp = fopen(ofname, "w"))==NULL){
				fprintf(stderr, ": faile\n");
				continue;
			}

		}
		if(ofp == NULL){
			fprintf(stderr, "skip: cl=%d because ofp=NULL\n", cl);
			continue;
		}
		if((err=fwrite(buf, BLOCKSIZE, 1, ofp))!=1){
			fprintf(stderr, "fwrite: error=%d, skip cl=%d\n", err, cl);
			continue;
		}
		oflen += BLOCKSIZE;
	}
	return 0;
}
