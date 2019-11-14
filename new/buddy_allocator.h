#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
int nthOfLevel(int idx){
	int Idxlog = (int)floor(log2(idx));
	int powerOf2 = pow(2,Idxlog);
	int ret = idx%powerOf2;
	return ret;
}
int newIdx(int oldIdx, int sizeChange) //corresponding index when adding levels to a tree
{
	int nth = nthOfLevel(oldIdx);
	int oldLevel = levelIdx(oldIdx);
	int newLevel = oldLevel + sizeChange;
	int ret = pow(2,newLevel) + nth;
	return ret;
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
		//printf("error, requested %d bytes, there is not enough memory\n",size);
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
int BuddyAllocator_unFree(BuddyAllocator* alloc, int idx){
	int level = levelIdx(idx);
	clearBit(alloc->tree,idx);
	//all parents and all sons are not free anymore
	int i;
	for (i = idx; i > 0; i=parentIdx(i))
	{
		clearBit(alloc->tree,i);
	}
	int nSons = pow(2,alloc->num_levels-level) - 1;
	int stack[(int)log2(nSons)+2];
	int currentNode;
	stack[0]=idx;
	int stackTop = 0;
	clearBit(alloc->tree,stack[stackTop]);
	for (int i = 0; i < nSons; i++)
	{
		clearBit(alloc->tree,stack[stackTop]);
		if(leftSon(stack[stackTop]) < pow(2,alloc->num_levels))
		{
			int ogTop = stackTop;
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = rightSon(stack[ogTop]);
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = leftSon(stack[ogTop]);
		}
		else{
			stackTop--;
			if(stackTop<0) stackTop = (int)log2(nSons)+1;
			//printf("%d\n", stackTop);
		}
	}
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
	int stack[(int)log2(nSons)+2];
	int currentNode;
	stack[0]=idx;
	int stackTop = 0;
	clearBit(alloc->tree,stack[stackTop]);
	for (int i = 0; i < nSons; i++)
	{
		clearBit(alloc->tree,stack[stackTop]);
		if(leftSon(stack[stackTop]) < pow(2,alloc->num_levels))
		{
			int ogTop = stackTop;
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = rightSon(stack[ogTop]);
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = leftSon(stack[ogTop]);
		}
		else{
			stackTop--;
			if(stackTop<0) stackTop = (int)log2(nSons)+1;
			//printf("%d\n", stackTop);
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
	int stack[(int)log2(nSons)+2];
	int currentNode;
	stack[0]=idx;
	int stackTop = 0;
	clearBit(alloc->tree,stack[stackTop]);
	for (int i = 0; i < nSons; i++)
	{
		setBit(alloc->tree,stack[stackTop]);
		if(leftSon(stack[stackTop]) < pow(2,alloc->num_levels))
		{
			int ogTop = stackTop;
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = rightSon(stack[ogTop]);
			stackTop++;
			if(stackTop>=(int)log2(nSons)+2) stackTop=0;
			//printf("%d\n", stackTop);
			stack[stackTop] = leftSon(stack[ogTop]);
		}
		else{
			stackTop--;
			if(stackTop<0) stackTop = (int)log2(nSons)+1;
			//printf("%d\n", stackTop);
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
void transferToNewAllocator(BuddyAllocator* oldAllocator, BuddyAllocator* newAllocator)
{
	//here is assumed that newAllocator was already initialized
	int i = 0;
	int oldAllocatorNNodes = pow(2,oldAllocator->num_levels);//this is actually NNodes+1
	for (int i = 1; i < oldAllocatorNNodes; i++);
	{
		if(!getBit(oldAllocator->tree,i))
		{
			BuddyAllocator_unFree(newAllocator, newIdx(i, newAllocator->num_levels-oldAllocator->num_levels));
			int* overwrite = chunkGivenIndex(newAllocator, i); //we overwrite the old index, written at the beginning of the chunk
			*overwrite = i;
		}
	}
}
void* myMalloc(BuddyAllocator* alloc, int size){
	void* ret = BuddyAllocator_malloc(alloc,size);
	BuddyAllocator* oldAlloc = alloc;
	while(ret == NULL)
	{
		int toIncrement = 1;
		while(size > (alloc->min_bucket_size)*pow(2,(alloc->num_levels)+toIncrement)){ //if we already know the new allocator is going to be too small there is no point in trying
			toIncrement++;
		}
		//initialize new allocator
		transferToNewAllocator(oldAlloc, newAlloc);
		//sono troppo stanco per continuare, ecco quello che rimane da fare
		//creare una struct che contenga il buddy allocator attuale, a questa funzione si passa
		//quella struct invece di un buddy allocator. Quando si alloca il nuovo allocator la struct si
		//prende quello come nuovo allocator e si scorda quello vecchio.
		//per il resto di queesta funzione, non appena si reisce a portare a termine la buddyallocatormalloc
		//e si è eventualmente cambiato il buddyAllocator atturale la funzione restituisce il 
		//chunk di memoria della malloc riuscita.
		//ricordare che il buffer del primo Buddy allocator, e di tutti quelli che vengono dopo è il buffer dato da
		//sbrk al momento di creare il primo Buddy (BuddyAllocator1 = sbrk(millemila))
		//quando si crea un nuovo BuddyAllocator si deve ripetere la sbrk
		//Per l'altra implementazione basta creare un nuovo buddy allocator ogni volta che non
		//si riesce a mallocare e la myMalloc di quella implementazione funziona su una lista di
		//Buddy allocator invece che su uno solo. Ogni volta che si crea un nuovo buddy allocator si
		//aggiunge alla lista
	}
	
}
////////////////////////////////////////////////////////////////////////
//note, level 0 exists, so if there are 8 levels the last number is 7
//on the other hand node 0 does not exist, if the node number 0 is returned there was an error
