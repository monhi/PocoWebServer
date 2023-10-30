//#include "stdafx.h"
#include "Cmd.h"
#include "general.h"


CCmd::CCmd()
{

}


CCmd::~CCmd()
{
}

int CCmd::ParseCommand(u8* buffer, int len, params_t*& params)
{

	if (
		(buffer[0] == 0xAA)
		&&
		(buffer[1] == 0xBB)
		&&
		(buffer[2] == 0xCC)
		&&
		(buffer[3] == 0xDD)
	   )
	{
		recv_channel  = (buffer[4]) +
						(((u32)(buffer[5])) * 256) +
						(((u32)(buffer[6])) * 256 * 256) +
						(((u32)(buffer[7])) * 256 * 256 * 256);
		recv_cmd	  = (buffer[8]) +
						(((u32)(buffer[9])) * 256) +
						(((u32)(buffer[10])) * 256 * 256) +
						(((u32)(buffer[11])) * 256 * 256 * 256);

		recv_paramLen = buffer[12] +
						(((u32)(buffer[13])) * 256) +
						(((u32)(buffer[14])) * 256 * 256) +
						(((u32)(buffer[15])) * 256 * 256 * 256);
		params = (params_t*)(&buffer[PARAM_START_OFFSET]);
		return recv_paramLen + PARAM_START_OFFSET;
	}
	else
	{
		recv_cmd = ALIVE;
		return FAILURE;
	}
}

int CCmd::GetChannel()
{
	return recv_channel;
}

int CCmd::CreateCommand(u32 cmd, u32 channel, params_t* pms)
{

	memset(cmd_buff, 0, COMMAND_LENGTH);

	cmd_buff[0] = 0xAA;
	cmd_buff[1] = 0xBB;
	cmd_buff[2] = 0xCC;
	cmd_buff[3] = 0xDD;

	cmd_buff[4] = (channel)& 0xFF;
	cmd_buff[5] = (channel >> 8) & 0xFF;
	cmd_buff[6] = (channel >> 16) & 0xFF;
	cmd_buff[7] = (channel >> 24) & 0xFF;

	cmd_buff[8] = (cmd)& 0xFF;
	cmd_buff[9] = (cmd >> 8) & 0xFF;
	cmd_buff[10] = (cmd >> 16) & 0xFF;
	cmd_buff[11] = (cmd >> 24) & 0xFF;

	switch (cmd)
	{
	case ALIVE:
		send_paramLen = 0;
		break;
   default:
		return 0;
	}

	cmd_buff[12] = (send_paramLen)& 0xFF;
	cmd_buff[13] = (send_paramLen >> 8) & 0xFF;
	cmd_buff[14] = (send_paramLen >> 16) & 0xFF;
	cmd_buff[15] = (send_paramLen >> 24) & 0xFF;
	return (PARAM_START_OFFSET + send_paramLen);
}

int	CCmd::GetCommandBuffer(u8* buffer)
{
	memcpy(buffer, cmd_buff, PARAM_START_OFFSET + send_paramLen);
	return PARAM_START_OFFSET + send_paramLen;
}
