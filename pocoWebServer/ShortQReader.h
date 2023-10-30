#ifndef _FLOAT_Q_READER_H_
#define _FLOAT_Q_READER_H_

#include "ShortQWriter.h"

class CShortQReader
{
public:
		 CShortQReader		();
		 ~CShortQReader		();
	int  	 GetQSize		();
	void 	 EmptyQueue		();
	void  	 SetWriterQ		(CShortQWriter* writer){pWriter = writer;}
	void 	 Remove			(short* arr,int len);
private:
	CShortQWriter* 			pWriter;
	int  					read_index;
};

#endif
