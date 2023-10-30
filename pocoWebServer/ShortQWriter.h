#ifndef _FLOAT_Q_WRITER_H_
#define _FLOAT_Q_WRITER_H_

#include "general.h"
#include <mutex>
#include <atomic>

#define 	FLOAT_Q_LENGTH     16*8000


class CShortQWriter
{
public:
	CShortQWriter();
	~CShortQWriter();
	void 	Insert	 (short* array,int len);
	u32		GetIndex();
	void 	GetData	 (short* array,int s_idx,int len);
private:
	std::mutex			m_mutex;
	short 				Q[FLOAT_Q_LENGTH];
	std::atomic<u32> 	index;
};

#endif

