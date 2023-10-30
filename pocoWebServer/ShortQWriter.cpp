#include "ShortQWriter.h"
#include <string.h>

CShortQWriter::CShortQWriter()
{
	index = 0;
	memset(Q,0,sizeof(short)*FLOAT_Q_LENGTH);
}

CShortQWriter::~CShortQWriter()
{
		
}

void CShortQWriter::Insert(short* array,int len)
{
	int temp;
	if(len > FLOAT_Q_LENGTH)
		return;
	m_mutex.lock();
	if(len+index < FLOAT_Q_LENGTH)	
	{
		memcpy(&Q[index],array,len*sizeof(short));		
		index += len;
	}
	else
	{
		temp = FLOAT_Q_LENGTH - index;
		memcpy(&Q[index], array		 ,temp*sizeof(short));		
		memcpy(&Q[    0],&array[temp],(len-temp)*sizeof(short));				
		index = len - temp;
	}
	m_mutex.unlock();
}

void CShortQWriter::GetData( short* arr,int s_idx,int len )
{
	m_mutex.lock();
      int temp = FLOAT_Q_LENGTH - s_idx;
      if(s_idx+len < FLOAT_Q_LENGTH)
      {
			memcpy(arr,&Q[s_idx],len*sizeof(short));
      }
      else
      {
			memcpy(arr,&Q[s_idx],temp*sizeof(short));
			memcpy(&arr[temp],Q ,(len-temp)*sizeof(short));
      }
	  m_mutex.unlock();
}

u32	CShortQWriter::GetIndex()
{ 
	return index; 
}
