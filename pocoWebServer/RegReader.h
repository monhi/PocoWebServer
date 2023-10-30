#pragma once
#include "General.h"
#include <string>
#include "IniHandler.h"
#include <mutex>


#define CHANNEL_VOICE_FORMAT_WAVE 0

#define CHANNEL_RECORDING_FORMAT_TELEPHONE  0
#define CHANNEL_RECORDING_FORMAT_RADIO		1

class CConfigReader
{
private:
	static			CConfigReader* m_pConfigReader;
					CConfigReader();
	CIniHandler		*m_pIni;
	void			ReadData();
	time_t			lastUpdate;
	std::mutex		m_mutex;
public:
	static			CConfigReader*	GetInstance();
	~CConfigReader();

	u32 			m_retention;
	u32				m_drives;
	std::string		m_db_username;
	std::string		m_db_password;
	int				m_channelActive[MAX_CHANNEL_NUMBER];
	std::string		m_channelName[MAX_CHANNEL_NUMBER];
	u32				m_channelFormat[MAX_CHANNEL_NUMBER];
	u32				m_channelSaving[MAX_CHANNEL_NUMBER];
	float			m_channelMu[MAX_CHANNEL_NUMBER];
	float			m_channelSD[MAX_CHANNEL_NUMBER];
	float			m_channelEN[MAX_CHANNEL_NUMBER];
};

