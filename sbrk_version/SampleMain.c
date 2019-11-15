#include "buddy_allocator.h"
#include <stdio.h>
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
AllocatorHolder holder;


int main (){
	
	AllocatorHolder_init(&holder, MIN_BUCKET_SIZE, BUDDY_LEVELS);
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	
	void* p1=myMalloc(&holder, 2000000);
	void* p2=myMalloc(&holder, 200000);
	myFree(&holder,p2);	
	void* p3=myMalloc(&holder, 200000);
	void* p4=myMalloc(&holder, 200000);
	void* p5=myMalloc(&holder, 4000000);
	myFree(&holder, p3);
	void* p9=myMalloc(&holder, 400000);
	myFree(&holder, p4);
	myFree(&holder, p5);
	myFree(&holder, p9);
	void* p8=myMalloc(&holder, 800000);
}
