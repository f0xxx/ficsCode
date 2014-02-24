#include "osdTools.h"
#include <assert.h>

SpaceAnalyse::SpaceAnalyse(SpaceNode &APar,SpaceNode &BPar):A(APar),B(BPar),isHaveSameSpace(false)
{
	if (A.length == 0 || B.length == 0)
	{
		return;
	}

	assert(A.length != 0);
	assert(B.length != 0);
	memset(&Same,0,sizeof(SpaceNode));
	memset(&ADiff,0,sizeof(SpaceNode)*2);
	memset(&BDiff,0,sizeof(SpaceNode)*2);

	if (A.start == B.start && A.length == B.length)
	{
		isHaveSameSpace = true;
		Same.start = A.start;
		Same.length = A.length;
		return;
	}

	if ((A.start>=B.start && A.start<B.start+B.length)||	\
		(A.start+A.length<=B.start+B.length && A.start+A.length>B.start)||	\
		(A.start<B.start && A.start+A.length>B.start+B.length))
	{
		isHaveSameSpace = true;
		if (A.start>=B.start && A.start<B.start+B.length)
		{
			Same.start = A.start;
			Same.length = (A.start + A.length > B.start + B.length)?(B.start + B.length - A.start):A.length;
			if (A.start>B.start)
			{
				BDiff[0].start = B.start;
				BDiff[0].length = A.start - B.start;
			}
			if (A.start + A.length > B.start + B.length)
			{
				ADiff[0].start = B.start + B.length;
				ADiff[0].length = (A.start + A.length)-(B.start + B.length);
			}
			else if (A.start + A.length < B.start + B.length)
			{
				BDiff[1].start = A.start + A.length;
				BDiff[1].length = (B.start + B.length)-(A.start + A.length);
			}
		}
		else if (A.start+A.length<=B.start+B.length && A.start+A.length>B.start)
		{
			Same.start = (A.start > B.start)?A.start:B.start;
			Same.length = A.start + A.length - Same.start;
			if (A.start+A.length<B.start+B.length)
			{
				BDiff[0].start = A.start+A.length;
				BDiff[0].length = (B.start + B.length)-(A.start + A.length);
			}
			if (A.start > B.start)
			{
				BDiff[1].start = B.start;
				BDiff[1].length = A.start - B.start;
			}
			else if (A.start < B.start)
			{
				ADiff[0].start = A.start;
				ADiff[0].length = B.start-A.start;
			}
		}
		else
		{
			Same.start = B.start;
			Same.length = B.length;
			ADiff[0].start = A.start;
			ADiff[0].length = B.start-A.start;
			ADiff[1].start = B.start+B.length;
			ADiff[1].length = (A.start+A.length)-(B.start+B.length);
		}
	}
}

unsigned int str2u32(const char *str)
{
    int i = 0;
	int len = strlen(str);
	int num = 0;

    while(i<len)
	{
		if(str[i]<'0' || str[i]>'9')
		{
			return -1;
		}
		num = num * 10 + (int)(str[i] - '0');
		++i;
	}
    return num;
}

