#include "TR4Connection.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Cmd.h"
#include <map>

CTR4Connection* CTR4Connection::m_Instance = nullptr;

CTR4Connection::CTR4Connection()
{
	mapper[REC_IDLE]			= "IDLE";
	mapper[REC_RECORDING_IN]	= "Input Recording";
	mapper[REC_RECORDING_OUT]	= "Output Recording";
	mapper[REC_WAITING]			= "Input Recording";
	mapper[REC_DISCONNECT]		= "DISCONNECT";
	mapper[REC_UNKNOWN]			= "UNKNOWN";
	mapper[REC_DISABLED]		= "DISABLED";

	for (int i = 0; i < MAX_CHANNEL_NUMBER; i++)
	{
		m_pAudioQ[i] = new CShortQWriter();
	}

	InitVars();
	ResetQ();
	m_pCmd = new CCmd();
	m_running = true;
	m_thread = std::thread(std::bind(&CTR4Connection::Routine, this));
}
void CTR4Connection::InitVars()
{
	int i;
	for (i = 0; i < MAX_CHANNEL_NUMBER; i++)
	{
		channelState[i] = REC_DISCONNECT;
		dial[i]=std::string();
		cid[i] =std::string();

	}
}

CTR4Connection::~CTR4Connection()
{
	m_running = false;
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	for (int i = 0; i < MAX_CHANNEL_NUMBER; i++)
	{
		delete m_pAudioQ[i];
	}
	delete m_pCmd;
}

CTR4Connection* CTR4Connection::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new CTR4Connection();
	}
	return m_Instance;
}

std::string CTR4Connection::GetDial(int ch)
{
	std::string temp;
	ch = ch%MAX_CHANNEL_NUMBER;
	mtx[ch].lock();
	temp = dial[ch];
	mtx[ch].unlock();
	return temp;
}

std::string CTR4Connection::GetCID(int ch)
{
	std::string temp;
	ch = ch%MAX_CHANNEL_NUMBER;
	mtx[ch].lock();
	temp = cid[ch];
	mtx[ch].unlock();
	return temp;
}

int CTR4Connection::GetState(int ch)
{
	int temp;
	ch = ch%MAX_CHANNEL_NUMBER;
	mtx[ch].lock();
	temp = channelState[ch];
	mtx[ch].unlock();
	return temp;
}



void CTR4Connection::Routine()
{
	bool established = false;
	StreamSocket		*ss1=nullptr;
	std::stringstream	 ss;
	int					 n;
	int					 length;
	int					cmd=0;
	int					channel=0;
	params_t*			pParam;
	char				bff[128];

	while (m_running)
	{
		try
		{
			InitVars();
			established = true;
			SocketAddress sa("localhost", 2077);
			ss1 = new StreamSocket(sa);
			ss1->setBlocking(false);
		}

		catch (...)
		{
			established = false;
			if (ss1)
			{
				delete ss1;
			}
		}
		ResetQ();
		while (established)
		{
			try
			{
				n = ss1->receiveBytes(buffer, sizeof(buffer));
				sprintf(bff, "received %d\n",n);
				//OutputDebugStringA(bff);
				if (n > 0)
				{
					AddQ(buffer, n);
					/*
					ss.str(std::string());
					for (i = 0; i < n; i++)
					{
						//ss <<std::hex << static_cast<unsigned char>(buffer[i]);
						if (
							(buffer[i] == 0xAA)
							&&
							(buffer[i + 1] == 0xBB)
							&&
							(i < n - 1)
							)
						{
							ss << std::endl;
						}
						ss << std::uppercase << std::hex << (int)(buffer[i]);
						ss << " ";
					}
					std::cout << ss.str();
					*/
				}
				else if (n == 0)
				{
					established = false;
					// set line states to disconnect 
					for (int i = 0; i < MAX_CHANNEL_NUMBER;i++)
					{
						mtx[channel].lock();
						channelState[i] = REC_DISCONNECT;
						mtx[channel].unlock();
					}


					if (ss1)
					{
						delete ss1;
						ss1 = nullptr;
					}
				}

				
				//if ( length <= GetQSize()&& (length > 0 ) )
				//if (length <= GetQSize() && (length > 0))
				while((length = ProcessQ()) > 0 )
				{
					if (length < RECV_BUFFER_SIZE)
					{

						sprintf(bff, "command %d length %d\n", cmd,length);
						//OutputDebugStringA(bff);

						memset(buffer, 0, RECV_BUFFER_SIZE);
						GetBuffer(buffer,length);
						ProcessCommand(buffer, length, cmd, channel, pParam);
						channel = channel % MAX_CHANNEL_NUMBER;
						switch (cmd)
						{
						case ALIVE:
							//std::cout << "ALIVE received" << std::endl;
						break;
						case SEND_CHANNEL_STATE:
							std::cout << "channel state " << channel << " is " << mapper[(pParam->state) % (REC_DISABLED + 1)] << std::endl;
							mtx[channel].lock();
							channelState[channel] = pParam->state;
							mtx[channel].unlock();
						break;
						case SEND_CHANNEL_CID:
							std::cout << "channel cid " << channel << " is " << pParam->str << std::endl;
							mtx[channel].lock();
							cid[channel] = pParam->str;
							mtx[channel].unlock();
						break;
						case SEND_CHANNEL_DIAL:
							std::cout << "channel dial " << channel << " is " << pParam->str << std::endl;
							mtx[channel].lock();
							dial[channel] = pParam->str;
							mtx[channel].unlock();
						break;
						case SEND_CHANNEL_VOICE:
							m_pAudioQ[channel]->Insert((short*)pParam->vbuf, MAX_VOICE/2);
						break;
						}
					}
					else
					{
						ResetQ();
					}
				}
			}
			catch (...)
			{
				established = false;
				delete ss1;
				ss1 = nullptr;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	}

	if (ss1)
	{
		delete ss1;
		ss1 = nullptr;
	}
}

void CTR4Connection::AddQ(u8* buffer, int len)
{
	int temp;
	if ((len + front) <  MAX_LENGTH)
	{
		memcpy(&StBuffer[front],buffer,len);
		front += len;
	}
	else
	{
		temp = MAX_LENGTH - front;
		if (temp)
		{
			memcpy(&StBuffer[front], buffer, temp);
		}
		if ((len - temp) > 0 )
		{
			memcpy(StBuffer, &buffer[temp], len - temp);
		}
		front = len - temp;
	}
}

int NEXT(int x, int i)
{
	int a = (x + i) % (MAX_LENGTH);
	return	a;
}

int CTR4Connection::GetQSize()
{
	if (front >= rear)
	{
		return front - rear;
	}
	return front + (MAX_LENGTH-rear);
}


int CTR4Connection::ProcessQ()
{
	int i;
	int length=0;
	int QSize = GetQSize();

	if (QSize >= 16)
	{
		i = rear;
		if (
			(StBuffer[i] == 0xAA)
			&& 
			(StBuffer[NEXT(i,1)]== 0xBB)
			&& 
			(StBuffer[NEXT(i,2)] == 0xCC)
			&& 
			(StBuffer[NEXT(i,3)] == 0xDD)
		   )
		{
			length = ((int)(StBuffer[NEXT(i, 12)]))				 + 
					 ((int)(StBuffer[NEXT(i, 13)]) * 256)		 +
					 ((int)(StBuffer[NEXT(i, 14)]) * 256 * 256)	 +
					 ((int)(StBuffer[NEXT(i, 15)]) * 256 * 256 * 256);
			if (length <= QSize)
			{
				return 16 + length;
			}
			else
			{
				return FAILURE;
			}
		}
		else
		{
			rear++;
			rear %= MAX_LENGTH;
		}
	}
	return FAILURE;
}
void CTR4Connection::ResetQ()
{
	front = rear = 0;
}

void CTR4Connection::GetBuffer(u8 *buffer, int length)
{
	int temp;
	if ( (rear + length) < MAX_LENGTH )
	{
		memcpy(buffer, &StBuffer[rear], length);
		rear += length;
	}
	else
	{
		temp = MAX_LENGTH - rear;
		memcpy(buffer, &StBuffer[rear], temp);
		memcpy(&buffer[temp],StBuffer , length - temp);
		rear = length - temp;
	}
}

void CTR4Connection::ProcessCommand(u8* buffer,int length, int &cmd, int &channel, params_t*& pms)
{
//	params_t*pParam = nullptr;
	m_pCmd->ParseCommand(buffer, length, pms);
	cmd		= m_pCmd->GetCommand();
	channel	= m_pCmd->GetChannel();
}
