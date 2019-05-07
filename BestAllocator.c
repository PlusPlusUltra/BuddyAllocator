#include <stdio.h>
#include <math.h>
//functions to work with integers as if they were bits
//i will store the bitmap as an array of integers and then work with the single bits of each integer
void setBit(int array[], int bitIndex) //sets the bit at index bitIndex
{
	int intIndex = bitIndex / (sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	array[intIndex] = array[intIndex] | flag;
}

void clearBit(int array[], int bitIndex) //clears the bit at index bitIndex
{
	int intIndex = bitIndex / (sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	flag = ~flag;
	array[intIndex] = array[intIndex] & flag;
}
int getBit(int array[], int bitIndex) //returns the value of the bit at index bitIndex. Can probably use char to return
{
	int intIndex = bitIndex /(sizeof(int)*8);
	int pos = bitIndex % (sizeof(int)*8);
	unsigned int flag = 1;
	flag = flag << pos;
	if (array[intIndex] & flag) return 1;
	else return 0;
}
//let's create the struct
typedef struct {
	int tree[];
	int num_levels;
	char* memory;
	int min_bucket_size;
} BuddyAllocator;
//tree functions
int level Idx(size_t idx)
{
	return (int)floor(log2(idx));
}
int buddyIdx(int idx){
	if (idx&0x1){ 
		return idx -1;
	}
	return idx+1;
}
int parentIdx(int idx){
	return idx/2;
}
int startIdx(int idx){
	return (idx-(1<<levelIdx(idx)));
}
int num_levels;
int min_bucket_size;

//allocator functions
void* BuddyAllocator_malloc(BuddyAllocator alloc, int size){
	//calculate max mem
	int mem_size= (1<<alloc->num_levels)*min_bucket_size;
	//calculate level for page
	int level = floor(log2(mem_size/(size+8)));
	
	//if the allocator does not have enough levels we just use the max level
	if (level > alloc->num_levels) level = num_levels;
	printf("requested: %d bytes, level %d \n",
	size, level);
	// get a buddy of the right size
	////////////////
	//write in the memory
	////////////
}
/*
int* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
	if (level<0) return 0;
	if (noBuddies()) { //no buddies on this level
		int* parent_ptr=BuddyAllocator_getBuddy(alloc, level-1); //recursion should be avoided, redo
		if (!parent_ptr) return 0;
		*/
int levelGivenSize(int size){
	
}
int isFree(int index){
}
int* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int size){
	level = levelGivenSize(size);
	int i;
	for (i = pow(2,level - 1); i < pow(2,level); i++)
	{
		//check if ith is free
		return //free block
	}
}
