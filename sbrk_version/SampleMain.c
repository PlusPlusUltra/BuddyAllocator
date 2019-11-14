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
	myFree(&holder,p1);
	void* p2=myMalloc(&holder, 200000);
	void* p3=myMalloc(&holder, 200000);
	void* p4=myMalloc(&holder, 200000);
	void* p5=myMalloc(&holder, 400000);
	void* p6=myMalloc(&holder, 100);
	void* p10=myMalloc(&holder,8000000);
	myFree(&holder, p1);
	myFree(&holder, p2);
	void* p7=myMalloc(&holder, 400000);
	myFree(&holder, p3);
	void* p9=myMalloc(&holder, 400000);
	myFree(&holder, p4);
	myFree(&holder, p5);
	myFree(&holder, p6);
	myFree(&holder, p7);
	myFree(&holder, p9);
	void* p8=myMalloc(&holder, 800000); 
}
