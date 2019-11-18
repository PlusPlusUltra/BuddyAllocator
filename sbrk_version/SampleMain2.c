#include "buddy_allocator.h"
#include <stdio.h>
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
AllocatorHolder holder;


int main (){
	
	AllocatorHolder_init(&holder, MIN_BUCKET_SIZE, BUDDY_LEVELS);
	///////////////////////////////////////////////////////////////////////////////////////////////
	void* p0=myMalloc(&holder,100);//p0
	myFree(&holder,p0);
	void* p1=myMalloc(&holder, 2000000); //p1
	void* p2=myMalloc(&holder, 200000); //p2
	void* p3=myMalloc(&holder, 20000);//p3
	myFree(&holder,p2);//p2
	myFree(&holder,p3);
}
