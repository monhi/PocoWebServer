#pragma once
#include "General.h"
#include "Cmd.h"



class CCmd
{
public:
	CCmd();
	~CCmd();

	int CreateCommand(u32 cmd, u32 channel, params_t* pms);
	int	GetCommandBuffer(u8* buffer);

	int ParseCommand(u8* buffer, int len, params_t*& params);

	int GetCommand()	{ return recv_cmd;		}
	int GetSerialNo()	{ return recv_serialNo;	}
	int GetChannel();
private:
	u8				cmd_buff[COMMAND_LENGTH];
	u32				recv_cmd				;
	u32				recv_serialNo			;
	u32				recv_paramLen			;
	u32				recv_channel			;
	u32				recv_resp				;
	u32				send_paramLen			;

};

