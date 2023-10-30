#pragma once

#include <thread>
#include <mutex>
#include "general.h"
#include "Cmd.h"
#include <map>
#include <string>
#include "ShortQWriter.h"


#include "poco/Exception.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"

using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::NetException;
using Poco::Exception;

#define  MAX_LENGTH			(1024*128) // 128 KB
#define RECV_BUFFER_SIZE	1536


using IntStr = std::map<int, std::string>;

class CTR4Connection
{
private:
	IntStr		mapper;
	int			channelState[MAX_CHANNEL_NUMBER];
	std::string dial[MAX_CHANNEL_NUMBER];
	std::string cid[MAX_CHANNEL_NUMBER];
	std::mutex  mtx[MAX_CHANNEL_NUMBER];
	u8			buffer[RECV_BUFFER_SIZE];

private:
				CTR4Connection();
	void InitVars();
	static CTR4Connection* m_Instance;
	bool m_running;
	std::thread m_thread;

public:
	~CTR4Connection();
	static CTR4Connection* GetInstance();
	std::string GetDial(int ch);
	std::string GetCID(int ch);
	int			GetState(int ch);
	void		Routine();
	CShortQWriter* m_pAudioQ[MAX_CHANNEL_NUMBER];
private: //Queue management
	u8		StBuffer[MAX_LENGTH];
	u32		front;
	u32     rear;
	CCmd	*m_pCmd;
	void    AddQ(u8* buffer, int len);
	int		GetQSize();
	int		ProcessQ();
	void	ResetQ();
	void	GetBuffer(u8 *buffer, int length);
	void	ProcessCommand(u8* buffer, int length, int &cmd, int &channel, params_t*& pms);
};

