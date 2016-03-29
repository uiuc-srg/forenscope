#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CON_MEM_RANGE 1048576 

uint32_t diff(char* file1, char* file2, int size, int pos) {
	uint32_t num_diff_bytes = 0;
    uint32_t i;
	for (i = 0; i < size; ++i) {
		if (*(file1+i) != *(file2+i)) {
//            		printf("differing pos %x \n", (pos+i));

            num_diff_bytes++;
		}
	}
	return num_diff_bytes;
}
int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Usage: diff filename1 filename2 pagesize\n");
		return 2;
	}

	uint32_t pagesize = atoi(argv[3]);
	uint64_t con_mem_pages = CON_MEM_RANGE / pagesize;
	uint64_t pos = 0;

	//printf("pagesize: %d\n", pagesize);
	char* file1_buf = (char*) malloc(pagesize * sizeof(char));
	char* file2_buf = (char*) malloc(pagesize * sizeof(char));

	uint64_t diff_pages = 0;
	uint64_t diff_pages_con_mem = 0;
	uint64_t num_bytes_diff_con_mem = 0;
	uint64_t num_bytes_diff = 0;
	uint64_t total_pages = 0;
	
	FILE* f1 = fopen(argv[1], "r");
	if (f1 == NULL) {
		printf("Failed to open file: %s", argv[1]);
		return 2;
	}


	FILE* f2 = fopen(argv[2], "r");
	if (f2 == NULL) {
		printf("Failed to open file: %s", argv[2]);
		return 2;
	}

	// get the first page
	size_t num_bytes_1 = fread(file1_buf, 1, pagesize, f1);	
	size_t num_bytes_2 = fread(file2_buf, 1, pagesize, f2);	

	while (num_bytes_1 == pagesize && num_bytes_2 == pagesize) {

		total_pages++;
        uint32_t num_diff_bytes = diff(file1_buf, file2_buf, pagesize, pos);
		if (num_diff_bytes != 0) {
			if (pos < CON_MEM_RANGE) {
				diff_pages_con_mem++;
				num_bytes_diff_con_mem += num_diff_bytes;
			} else {
				diff_pages++;
				num_bytes_diff += num_diff_bytes;
			}
            		//printf("differing page %x \n", (pos));
		
		}
		pos = pos + pagesize;
		num_bytes_1 = fread(file1_buf, 1, pagesize, f1);	
		num_bytes_2 = fread(file2_buf, 1, pagesize, f2);
	}

	printf("total pages: %lld\n", total_pages);
	printf("%lld, %lld %f\n", num_bytes_diff_con_mem, diff_pages_con_mem, (double)diff_pages_con_mem/(double)con_mem_pages);
	printf("%lld, %lld %f\n", num_bytes_diff, diff_pages, (double)diff_pages/(double)total_pages);
	fclose(f1);
	fclose(f2);
	free(file1_buf);
	free(file2_buf);
	return 0;
}


