
//functions to work with integers as if they were bits
//i will store the bitmap as an array of integers and then work with the single bits of each integer
void setBit(int[] array, int bitIndex) //sets the bit at index bitIndex
{
	int intIndex = bitIndex / sizeof(int);
	int pos = bitIndex % sizeof(int);
	unsigned int flag = 1;
	flag = flag << pos;
	array[intIndex] = array[intIndex] | flag;
}

void clearBit(int[] array, int bitIndex) //clears the bit at index bitIndex
{
	int intIndex = bitIndex / sizeof(int);
	int pos = bitIndex % sizeof(int);
	unsigned int flag = 1;
	flag = flag << pos;
	flag = ~flag;
	array[intIndex] = array[intIndex] & flag;
}
int getBit(int[] array, int bitIndex) //returns the value of the bit at index bitIndex. Can probably use char to return
{
	int intIndex = bitIndex /sizeof(int);
	int pos = bitIndex % sizeof(int);
	unsigned int flag = 1;
	flag = flag << pos;
	if (array[intIndex] & flag) return 1;
	else return 0;
}
