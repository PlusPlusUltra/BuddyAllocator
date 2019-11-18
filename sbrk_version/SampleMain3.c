#include "buddy_allocator.h"
#include <stdio.h>
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
AllocatorHolder holder;


int main (){
	
	AllocatorHolder_init(&holder, MIN_BUCKET_SIZE, BUDDY_LEVELS);
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	
	void* p1=myMalloc(&holder, 4000000);
	myFree(&holder,p1);
}
