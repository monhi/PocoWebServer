#include "RegReader.h"
#include <string.h>
#include <algorithm>
#include "general.h"
#include <time.h>
#include "base64.h"

#define COTTAGE_REGISTRY_1			"SOFTWARE\\Wow6432Node\\DSPCottage\\TR4\\Server"
#define COTTAGE_REGISTRY_2			"SOFTWARE\\DSPCottage\\TR4\\Server"


CConfigReader* CConfigReader::m_pConfigReader = NULL;


CConfigReader*	CConfigReader::GetInstance()
{
	if (m_pConfigReader == NULL)
	{
		
		m_pConfigReader = new CConfigReader();
	}
	m_pConfigReader->ReadData();
	return m_pConfigReader;
}

void CConfigReader::ReadData()
{
	int	 i;
	char key[32];
	char *keyPtr;
	char value[32];
	char *valuePtr;
	char section[32];


	time_t alpha = time(NULL);
	if (difftime(alpha, lastUpdate) < 30)
		return;

	m_mutex.lock();
	lastUpdate = alpha;

	m_pIni = new CIniHandler(SETTING_FILE_NAME);
	/////////////////////////////////////////////////////////// Getting Drive Info
	strcpy(key, "drive");
	keyPtr = &key[0];
	valuePtr = &value[0];
	char* defaultDrv = { "4" };
	m_pIni->GetValue("Server", keyPtr, valuePtr, defaultDrv);
	m_drives = atoi(valuePtr);
	if (m_drives <= 4)
	{
		m_drives = 4;
	}
	/////////////////////////////////////////////////////////// Getting 
	strcpy(key, "RetentionDays");
	keyPtr = &key[0];
	valuePtr = &value[0];
	char* defaultRetention = { "7" };
	m_pIni->GetValue("Server", keyPtr, valuePtr, defaultRetention);
	m_retention = atoi(valuePtr);
	if (m_retention <= 0)
	{
		m_retention = 7;
	}
	///////////////////////////////////////////////////////////
	strcpy(key, "DBUserName");
	keyPtr = &key[0];


	valuePtr = &value[0];
	char* defaultUserName = { "" };
	m_pIni->GetValue("Server", keyPtr, valuePtr, defaultUserName);
	m_db_username = valuePtr;

	strcpy(key, "DBPassword");
	keyPtr = &key[0];
	valuePtr = &value[0];
	char* defaultPassWord = { "" };

	m_pIni->GetValue("Server", keyPtr, valuePtr, defaultPassWord);
	std::string password = base64_decode(valuePtr);
	m_db_password = password;

	for (i = 0; i < MAX_CHANNEL_NUMBER; i++)
	{
		sprintf_s(section, "CH%d", i);
		////  CHANNEL NAME ////
		strcpy(key, "Name");
		keyPtr = &key[0];
		valuePtr = &value[0];
		m_pIni->GetValue(section, keyPtr, valuePtr, section);
		m_channelName[i] = valuePtr;
		//// Active Status ////
		strcpy(key, "Status");
		keyPtr = &key[0];
		valuePtr = &value[0];
		m_pIni->GetValue(section, keyPtr, valuePtr, "1");
		m_channelActive[i] = atoi(valuePtr);
	}
	delete m_pIni;
	m_mutex.unlock();
}

CConfigReader::CConfigReader()
{
	lastUpdate = 0;
	ReadData();
}


CConfigReader::~CConfigReader()
{
}
