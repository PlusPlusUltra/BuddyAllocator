#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#define MIN_LEVELS 9
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
void printTree(int* tree, int levels){
	for (int i = 0; i < levels; i++)
	{
		for (int j = 0; j < pow(2,i);j++)
		{
			printf("%d",getBit(tree,pow(2,i)+j));
		}
		printf("\n");
	}
	
}
//let's create the struct
typedef struct {
	int num_levels;
	char* memory;
	int min_bucket_size;
	int* tree;

} BuddyAllocator;
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);
typedef struct {
	BuddyAllocator* currentAllocator;
} AllocatorHolder;
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
int nthOfLevel(int idx){
	int Idxlog = (int)floor(log2(idx));
	int powerOf2 = pow(2,Idxlog);
	int ret = idx%powerOf2;
	return ret;
}
int isRightSideFree(int* tree){
	return getBit(tree,3);
}
int newIdx(int oldIdx, int sizeChange) //corresponding index when adding levels to a tree
{
	int nth = nthOfLevel(oldIdx);
	int oldLevel = levelIdx(oldIdx);
	int newLevel = oldLevel + sizeChange;
	int ret = pow(2,newLevel) + nth;
	return ret;
}
void recClearAllSons(int*tree, int levels, int father){
	clearBit(tree, father);
	if(leftSon(father)< pow(2,levels)){
		recClearAllSons(tree,levels,leftSon(father));
		recClearAllSons(tree,levels,rightSon(father));
	}
}
void recSetAllSons(int*tree, int levels, int father){
	setBit(tree, father);
	if(leftSon(father)< pow(2,levels)){
		recSetAllSons(tree,levels,leftSon(father));
		recSetAllSons(tree,levels,rightSon(father));
	}
}

//allocator functions
void* chunkGivenIndex(BuddyAllocator* alloc, int idx){
	int level = levelIdx(idx);
	int firstOfLevel = pow(2,level);
	int toAdd = alloc->min_bucket_size * (alloc->num_levels - level) * (idx - firstOfLevel);
	return alloc->memory + toAdd;
}
int sizeGivenLevels(int levels){ //you have to multiply this times the size of the smallest chunk
	//int ret = pow(2,(levels-1));
	int ret = pow(2,levels);
}
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
	//calculate max mem
	int mem_size= (1<<(alloc->num_levels))*alloc->min_bucket_size;
	//calculate level for page
	int level = floor(log2(mem_size/(size+8)));
	if(level<0){
		//printf("error, requested %d bytes, there is not enough memory\n",size);
		return NULL;
	}
	//if the allocator does not have enough levels we just use the max level
	if (level > (alloc->num_levels - 1)) level = alloc->num_levels - 1;
	printf("requested: %d bytes, level %d\n",
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
	printf("%p given\n",ret);
	//printTree(alloc->tree,alloc->num_levels);
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
int BuddyAllocator_unFree(BuddyAllocator* alloc, int idx){
	int level = levelIdx(idx);
	clearBit(alloc->tree,idx);
	//all parents and all sons are not free anymore
	int i;
	for (i = idx; i > 0; i=parentIdx(i))
	{
		clearBit(alloc->tree,i);
	}
	recClearAllSons(alloc->tree, alloc->num_levels,idx);
	
	return 0;
}
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
	if (level<0) return 0;
	int idx = minBuddy(alloc, level);
	if(idx == 0){
		return 0;
	}
	BuddyAllocator_unFree(alloc,idx);
	
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
	//#include <sys/resource.h>
	//register long int rsp asm("rsp");
	//printf("%lx",rsp);
	//printf("1\n");
	
	recSetAllSons(alloc->tree,alloc->num_levels,idx2);
	
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
	//printTree(alloc->tree,alloc->num_levels);
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
void AllocatorHolder_init(AllocatorHolder* holder, int minBucket, int numLevels){
	printf("Initializing first allocator\n");
	int numberOfBuddies = (int)(pow(2,numLevels)/(sizeof(int)*8));
	int* bitmap = malloc(numberOfBuddies*sizeof(int));
	int firstBuffer = sizeGivenLevels(numLevels)*minBucket;
	BuddyAllocator* firstAlloc = malloc(sizeof(BuddyAllocator));
	char* mem = sbrk(firstBuffer);
	BuddyAllocator_init(firstAlloc, mem, minBucket, numLevels, bitmap);
	holder->currentAllocator = firstAlloc;
}
void transferToNewAllocator(BuddyAllocator* oldAllocator, BuddyAllocator* newAllocator)
{
	//here is assumed that newAllocator was already initialized
	int oldAllocatorNNodes = pow(2,oldAllocator->num_levels);//this is actually NNodes+1
	int i;	
	for (i = 1; i < oldAllocatorNNodes; i++)
	{
		if(!getBit(oldAllocator->tree,i))
		{
			BuddyAllocator_unFree(newAllocator, newIdx(i, newAllocator->num_levels-oldAllocator->num_levels));
			int* overwrite = chunkGivenIndex(newAllocator, i); //we overwrite the old index, written at the beginning of the chunk
			*overwrite = i;
		}
	}
	//printTree(oldAllocator->tree,oldAllocator->num_levels);
	//printTree(newAllocator->tree,newAllocator->num_levels);

}
void transferToNewAllocatorFree(BuddyAllocator* oldAllocator, BuddyAllocator* newAllocator, int node)
{
	//here is assumed that newAllocator was already initialized
	int oldAllocatorNNodes = pow(2,oldAllocator->num_levels);//this is actually NNodes+1
	if(node == 0){
		if(!getBit(oldAllocator->tree,node)){
			BuddyAllocator_unFree(newAllocator, newIdx(node, newAllocator->num_levels-oldAllocator->num_levels));
			int* overwrite = chunkGivenIndex(newAllocator, node); //we overwrite the old index, written at the beginning of the chunk
			*overwrite = node;
		}
		transferToNewAllocatorFree(oldAllocator, newAllocator, leftSon(node));
	 }
	else{
		if(!getBit(oldAllocator->tree,node)){
			printf("XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
			BuddyAllocator_unFree(newAllocator, newIdx(node, newAllocator->num_levels-oldAllocator->num_levels));
			int* overwrite = chunkGivenIndex(newAllocator, node); //we overwrite the old index, written at the beginning of the chunk
			*overwrite = node;
		}
		if(leftSon(node) < pow(2,oldAllocator->num_levels)){
			transferToNewAllocatorFree(oldAllocator, newAllocator, leftSon(node));
			transferToNewAllocatorFree(oldAllocator, newAllocator, rightSon(node));
		}
	}
}

void* myMalloc(AllocatorHolder* holder, int size){
	BuddyAllocator* oldAlloc = holder->currentAllocator;
	void* ret = BuddyAllocator_malloc(oldAlloc,size);
	while(!ret)
	{
		int toIncrement = 1;
		while(size > (oldAlloc->min_bucket_size)*sizeGivenLevels(oldAlloc->num_levels+toIncrement)){ //if we already know the new allocator is going to be too small there is no point in trying
			toIncrement++;
		}
		oldAlloc = holder->currentAllocator;
		int newLevel = oldAlloc->num_levels + toIncrement;
		//initialize new allocator
		BuddyAllocator* newAlloc = malloc(sizeof(BuddyAllocator));
		int NewNumberOfBuddies = (int)(pow(2,newLevel)/(sizeof(int)*8));
		int levelChange = newLevel - oldAlloc->num_levels;
		int bufferDifference = (sizeGivenLevels(newLevel)-sizeGivenLevels(oldAlloc->num_levels)) * oldAlloc->min_bucket_size;
		sbrk(bufferDifference);
		int* newBitmap = malloc(NewNumberOfBuddies*sizeof(int));
		BuddyAllocator_init(newAlloc, oldAlloc->memory, oldAlloc->min_bucket_size, newLevel,newBitmap);
		transferToNewAllocator(oldAlloc, newAlloc);
		printf("Tree size increase, new number of levels: %d\n",newLevel);
		holder->currentAllocator = newAlloc;
		free(oldAlloc->tree);
		free(oldAlloc);
		ret = BuddyAllocator_malloc(newAlloc, size);
		//using the actual malloc for allocating BuddyAllocators and trees should not be a problem, in my expereiments
		//i saw that it is difficult that that changes the program break
	}
	return ret;
}
//note that i am using the actual malloc here. It looks a little off, but this is the reason.
		//another option would be to permanently have a few buddies occupied for storing the variables
		//necessary for the allocators, but that would be kind of annoying for testing the allocator.
		//I think that what is important is that it could be done. Also, using the malloc here will not
		//interfere with the allocators. In fact there are for sure some parts of the heap that are not
		//storing anything that are lower that the start of the buffer of the allocators. So the malloc
		//will not increase the program break, and when i will need a very big chunk of memory the malloc will
		//use mmap and not sbrk to allocate it. I have done multiple tests and experiments, and the only time
		//when using malloc increased the program break was when trying to allocate a lot of not-too-big
		//chunks of memory, which is never being done here.
		
void myFree(AllocatorHolder* holder, void* mem)
{
	BuddyAllocator_free(holder->currentAllocator, mem);
	while(isRightSideFree(holder->currentAllocator->tree) && holder->currentAllocator->num_levels > MIN_LEVELS)
	{
		BuddyAllocator* oldAlloc = holder->currentAllocator;
		BuddyAllocator* newAlloc = malloc(sizeof(BuddyAllocator));
		int newLevel = oldAlloc->num_levels - 1;
		int NewNumberOfBuddies = (int)(pow(2,newLevel)/(sizeof(int)*8));
		int levelChange = -1;
		int bufferDifference = (sizeGivenLevels(newLevel)-sizeGivenLevels(oldAlloc->num_levels)) * oldAlloc->min_bucket_size;
		sbrk(bufferDifference);
		int* newBitmap = malloc(NewNumberOfBuddies*sizeof(int));
		BuddyAllocator_init(newAlloc, oldAlloc->memory, oldAlloc->min_bucket_size, newLevel,newBitmap);
		transferToNewAllocatorFree(oldAlloc, newAlloc,1);
		printf("Tree size decrease, new number of levels: %d\n",newLevel);
		holder->currentAllocator = newAlloc;
		free(oldAlloc->tree);
		free(oldAlloc);
	}
	printf("root is %d\n",getBit(holder->currentAllocator->tree,1));
}
////////////////////////////////////////////////////////////////////////
//note, level 0 exists, so if there are 8 levels the last number is 7
//on the other hand node 0 does not exist, if the node number 0 is returned there was an error
