#include "ShortQReader.h"
#include <stdlib.h>

CShortQReader::CShortQReader()
{
	pWriter 	= NULL	;
	read_index	= 0		;
}

CShortQReader::~CShortQReader()
{
	
}

int CShortQReader::GetQSize()
{
	int temp = pWriter->GetIndex();	
	int temp2;
	if(read_index <= temp)
	{
		return temp-read_index;
	}
	else
	{
		temp2 = FLOAT_Q_LENGTH-read_index;
		return (temp2+temp);
	}
}

void CShortQReader::EmptyQueue()
{
    //read_index = pWriter->GetIndex()-(SAMPLE_RATE*2);
	read_index = pWriter->GetIndex();
	if( read_index < 0 )
	{
		read_index += FLOAT_Q_LENGTH;
	}
}

void CShortQReader::Remove(short* arr,int len)
{
      pWriter->	GetData(arr,read_index,len);
      read_index += len;
      read_index %= FLOAT_Q_LENGTH;
}
