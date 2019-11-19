#include "buddy_allocator.h"
#include <stdio.h>
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
AllocatorHolder holder;


int main (){
	
	AllocatorHolder_init(&holder, MIN_BUCKET_SIZE, BUDDY_LEVELS);
	///////////////////////////////////////////////////////////////////////////////////////////////
	//void* p1=myMalloc(&holder, 1000000000); as of now the maximum level is 19, reached with this
	
	void* p1 = myMalloc(&holder, 100);
	void* p2 = myMalloc(&holder, 2000000);
}
