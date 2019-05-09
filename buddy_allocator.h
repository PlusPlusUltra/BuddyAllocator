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
	int num_levels;
	char* memory;
	int min_bucket_size;
	int* tree;

} BuddyAllocator;
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);
//tree functions
int levelIdx(size_t idx)
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
int leftSon(int idx){
	return idx*2;
}
int rightSon(int idx){
	return idx*2 + 1;
}

//allocator functions
void* chunkGivenIndex(BuddyAllocator* alloc, int idx){
	int level = levelIdx(idx);
	int firstOfLevel = pow(2,level);
	int toAdd = alloc->min_bucket_size * (alloc->num_levels - level) * (idx - firstOfLevel);
	return alloc->memory + toAdd;
}
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
	//calculate max mem
	int mem_size= (1<<(alloc->num_levels))*alloc->min_bucket_size;
	//calculate level for page
	int level = floor(log2(mem_size/(size+8)));
	if(level<0){
		printf("error, requested %d bytes, there is not enough memory\n",size);
		return NULL;
	}
	//if the allocator does not have enough levels we just use the max level
	if (level > (alloc->num_levels - 1)) level = alloc->num_levels - 1;
	printf("requested: %d bytes, level %d \n",
	size, level);
	// get a buddy of the right size
	int buddy = BuddyAllocator_getBuddy(alloc,level);
	if(buddy==0){
		return NULL;
	}
	//write in the memory
	void* ret = chunkGivenIndex(alloc, buddy);
	int* forLater = (int*)ret;
	ret += 8;
	*forLater = buddy;
	return ret;
}
int minBuddy(BuddyAllocator* alloc, int level) //find the index of the smallest available buddy
{
	int i;
	for (i = pow(2,level+1) - 1; i > 0; i--)
	{
		if (getBit(alloc->tree, i)) return i;
	}
	printf("error, no buddy found\n");
	return 0;
	
}
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
	if (level<0) return 0;
	int idx = minBuddy(alloc, level);
	if(idx == 0){
		return 0;
	}
	clearBit(alloc->tree,idx);
	//all parents and all sons are not free anymore
	int i;
	for (i = idx; i > 0; i=parentIdx(i))
	{
		clearBit(alloc->tree,i);
	}
	int nSons = pow(2,alloc->num_levels-level) - 1;
	int toFree[nSons];
	int currentNode;
	toFree[0]=idx;
	int writeHere = 1;
	for (currentNode = 0; currentNode < nSons; currentNode++)
	{
		clearBit(alloc->tree,toFree[currentNode]);
		if(leftSon(toFree[currentNode]) < pow(2,alloc->num_levels))
		{
			toFree[writeHere] = leftSon(toFree[currentNode]);
			writeHere++;
			toFree[writeHere] = rightSon(toFree[currentNode]);
			writeHere++;
		}
	}
	return idx;
}
void releaseBuddy (BuddyAllocator* alloc, int idx)
{
	int idx2 = idx; //i need idx to free descendants as well
	while (idx > 0){
		setBit(alloc->tree,idx);
		if(getBit(alloc->tree,buddyIdx(idx))){
			idx = parentIdx(idx);
		}
		else break;
	}
	int nSons = pow(2,alloc->num_levels-levelIdx(idx2)) - 1;
	int toFree[nSons];
	int currentNode;
	toFree[0]=idx2;
	int writeHere = 1;
	for (currentNode = 0; currentNode < nSons; currentNode++)
	{
		setBit(alloc->tree,toFree[currentNode]);
		if(leftSon(toFree[currentNode]) < pow(2,alloc->num_levels))
		{
			toFree[writeHere] = leftSon(toFree[currentNode]);
			writeHere++;
			toFree[writeHere] = rightSon(toFree[currentNode]);
			writeHere++;
		}
	}
}
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
	if(!mem){
		printf("error, %p is a NULL pointer\n", mem);
		return;
	}
	printf("freeing %p\n", mem);
	mem = mem - 8;
	int* retr = (int*) mem;
	int idx = *retr;
	releaseBuddy(alloc, idx);
}
void BuddyAllocator_init(BuddyAllocator* alloc, char* mem, int minBucket, int numLevels, int* bitmap){
	//the number of levels, the minimum bucket size, the amount of memory and the amount 
	//of buddies in the tree need to be consistent, the chunk of memory bitmap points to needs to be large enough
	//an example of it is in the sample main
	alloc->memory = mem;
	alloc->min_bucket_size = minBucket;
	alloc->num_levels = numLevels;
	int numberOfBuddies = (int)(pow(2,numLevels));
	int i;
	for (i = 0; i < numberOfBuddies; i++)
	{
		setBit(bitmap,i);
	}
	alloc->tree = bitmap;

}
////////////////////////////////////////////////////////////////////////
//note, level 0 exists, so if there are 8 levels the last number is 7
//on the other hand node 0 does not exist, if the node number 0 is returned there was an error
