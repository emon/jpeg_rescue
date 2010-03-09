ALLTARGETS=jpeg_rescue jpeg_truncate

all: $(ALLTARGETS)

jpeg_rescue: jpeg_rescue.o
	$(CC) -o $@ $>

jpeg_truncate: jpeg_truncate.o
	$(CC) -o $@ $>


clean:
	rm *.o $(ALLTARGETS)

