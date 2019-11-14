#include "buddy_allocator.h"
#include <stdio.h>
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))
char memory[MEMORY_SIZE];
BuddyAllocator alloc;


int main (){
	int numberOfBuddies = (int)(pow(2,BUDDY_LEVELS)/(sizeof(int)*8));
	int bitmap[numberOfBuddies];
	BuddyAllocator_init(&alloc, memory, MIN_BUCKET_SIZE, BUDDY_LEVELS,bitmap);
	///////////////////////////////////////////////////////////////////////////////////////////////
	void* p1=BuddyAllocator_malloc(&alloc, 2000000);
	void* p2=BuddyAllocator_malloc(&alloc, 200000);
	void* p3=BuddyAllocator_malloc(&alloc, 200000);
	void* p4=BuddyAllocator_malloc(&alloc, 200000);
	void* p5=BuddyAllocator_malloc(&alloc, 400000);
	void* p6=BuddyAllocator_malloc(&alloc, 100);
	BuddyAllocator_free(&alloc, p1);
	BuddyAllocator_free(&alloc, p2);
	void* p7=BuddyAllocator_malloc(&alloc, 400000);
	BuddyAllocator_free(&alloc, p3);
	void* p9=BuddyAllocator_malloc(&alloc, 400000);
	BuddyAllocator_free(&alloc, p4);
	BuddyAllocator_free(&alloc, p5);
	BuddyAllocator_free(&alloc, p6);
	BuddyAllocator_free(&alloc, p7);
	BuddyAllocator_free(&alloc, p9);
	void* p8=BuddyAllocator_malloc(&alloc, 800000);
}
