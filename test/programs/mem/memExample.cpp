// g++ -g -o memExample memExample.cpp
// collectionTool --numBE 1  --program "./memExample" --collector mem
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main(int argc, char* argv[]) {

    void* mallocPtr = malloc(123);
    printf("mallocPtr=%p\n", mallocPtr);
    free(mallocPtr);

    char* charPtr = new char;
    printf("charPtr=%p\n", charPtr);
    delete charPtr;

    void* callocPtr = calloc(10,10);
    printf("callocPtr=%p\n", callocPtr);
    free(callocPtr);

    int *first = NULL;
    int *second, i;
    for(i==0; i<5; ++i) {
	second = (int*) realloc (first, i+1 * sizeof(int));

	if (second!=NULL) {
	    first=second;
	    first[i]=10;
	} else {
	    free (first);
	    fprintf(stderr,"Error (re)allocating memory\n");
	    exit (1);
	}
    }

    void* memalignPtr = NULL;
    memalignPtr = memalign(1024, 1024*4);
    printf("memalignPtr=%p\n", memalignPtr);
    free(memalignPtr);

    void* posix_memalignPtr = NULL;
    int retval = posix_memalign(&posix_memalignPtr, 1024, 1024*4);
    printf("posix_memalignPtr=%p\n", posix_memalignPtr);
    free(posix_memalignPtr);
}
