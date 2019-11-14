#include <stdio.h>
#include <math.h>

int levelIdx(size_t idx)
{
	return (int)floor(log2(idx));
}

int nthOfLevel(int idx){
	int Idxlog = (int)floor(log2(idx));
	int powerOf2 = pow(2,Idxlog);
	int ret = idx%powerOf2;
	return ret; //provare per numeri piccoli
}

int main()
{
	int oldIdx = 10;
	int sizeChange = -2;
	int nth = nthOfLevel(oldIdx);
	int oldLevel = levelIdx(oldIdx);
	int newLevel = oldLevel + sizeChange;
	int ret = pow(2,newLevel) + nth;
	return ret;
}
