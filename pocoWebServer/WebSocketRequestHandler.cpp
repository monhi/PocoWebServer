#include "WebSocketRequestHandler.h"
#include <string.h>
#include "TR4Connection.h"
#include <sstream>
#include <string>
#include "base64.h"
#include <chrono>


WebSocketRequestHandler::WebSocketRequestHandler()
{
	int i;
	CTR4Connection*	m_tr4 = CTR4Connection::GetInstance();
	for ( i = 0; i < MAX_CHANNEL_NUMBER; i++ )
	{
		lastState[i] = -1;
		lastcid[i]	 = "";
		lastdial[i]  = "";
		m_rQ[i] = new CShortQReader();
		m_rQ[i]->SetWriterQ(m_tr4->m_pAudioQ[i]);
	}

	idx = 0;
	m_time = 1;
	m_LiveChannel = 0;	
	m_LiveCounter = 0;
	ChNumber = MAX_CHANNEL_NUMBER - 1;


}


WebSocketRequestHandler::~WebSocketRequestHandler()
{
	int i;
	for (i = 0; i < MAX_CHANNEL_NUMBER; i++)
	{
		delete m_rQ[i];
	}
}

std::ostringstream WebSocketRequestHandler::CreateJSON(char* cmd, int channel, char* param,int time)
{
	std::ostringstream ss;
	ss.clear();
	//ss << "{ \"COMMAND\": \"" << cmd << "\", \"CHANNEL\":"<< channel <<", \"PARAM\":\"" << param <<"\"}" ;
	ss << "{ \"COMMAND\": \"" << cmd << "\", \"CHANNEL\":" << channel << ", \"TIME\":" << time <<", \"PARAM\":\"" << param << "\"}";
	return ss;
}

void WebSocketRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application&			app = Application::instance();
	WebSocket				ws(request, response);
	PingTimer				= 0;
	ChNumber				= MAX_CHANNEL_NUMBER - 1;
	state					= 0;
	m_LiveChannel			= 0;

	app.logger().information("WebSocket connection established.");


	ws.setBlocking(false);

	do
	{
		try
		{
			err = false;
			flags = 0;
			n = 0;
			n = ws.receiveFrame(buffer, sizeof(buffer), flags);
		}
		catch (...)
		{
			err = true;
		}

		if (err == true)
		{
			// do nothing yet.
		}
		else
		{
			int ret =ProcessReceive(ws);
			if (ret == BREAK)
			{
				break;
			}					
			else if (ret == CONTINUE)
			{
				continue;
			}
		}
		ProcessSignaling(ws);
		ProcessPing(ws);
		ProcessSendLiveVoice(ws);
		Sleep(16);
		PingTimer += 16;
	}
	while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
	app.logger().information("WebSocket connection closed.");
}

void WebSocketRequestHandler::ProcessPing(WebSocket ws)
{
	std::string alpha;
	int n;
	if (PingTimer >= PING_TIME)
	{
		// send ping message to 
		alpha = "ping message";
		flags = WebSocket::FRAME_OP_PING + 128;
		try
		{
			PingTimer = 0;
			n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
		}
		catch (...)
		{
			err = true;
		}
	}
}

void WebSocketRequestHandler::ProcessSignaling(WebSocket ws)
{
	int					chState;
	std::string			dial;
	std::string			cid;
	std::ostringstream	ss;
	CTR4Connection*		m_tr4 = CTR4Connection::GetInstance();
	std::string			alpha;

	ChNumber++;
	ChNumber %= MAX_CHANNEL_NUMBER;
	if ( ChNumber == 0 )
	{
		state++;
		state %= 3;
	}

	switch (state)
	{
	case 0:// sending state;
	{
		chState = m_tr4->GetState(ChNumber);
		ss.str("");
		ss.clear();
		if (chState != lastState[ChNumber])
		{
			//ss << "CMD " << ChNumber << " STATE ";
			switch (chState)
			{
			case REC_IDLE:
				//ss << "ONHOOK";
				ss = CreateJSON("STATE", ChNumber, "ONHOOK",0);
				break;
			case REC_RECORDING_IN:
			case REC_WAITING:
				//ss << "OFFHOOKIN";
				ss = CreateJSON("STATE", ChNumber, "OFFHOOKIN",0);
				break;
			case REC_RECORDING_OUT:
				//ss << "OFFHOOKOUT";
				ss = CreateJSON("STATE", ChNumber, "OFFHOOKOUT",0);
				break;
			case REC_DISCONNECT:
				//ss << "DISCONNECT";
				ss = CreateJSON("STATE", ChNumber, "DISCONNECT",0);
				break;
			case REC_DISABLED:
				//ss << "DISABLED";
				ss = CreateJSON("STATE", ChNumber, "DISABLED",0);
				break;
			}
			//ss << " ;";
			alpha = ss.str();
			n = ss.str().length();
			lastState[ChNumber] = chState;
			flags = WebSocket::FRAME_OP_TEXT + 128;
			try
			{
				n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
				PingTimer = 0;
			}
			catch (...)
			{
				err = true;
			}
		}
	}
	break;
	case 1:// dial
	{
		dial = m_tr4->GetDial(ChNumber);
		ss.str("");
		ss.clear();
		if (dial != lastdial[ChNumber])
		{

			ss = CreateJSON("DIAL", ChNumber, (char*)dial.c_str(),0);
			n = ss.str().length();
			lastdial[ChNumber] = dial;
			alpha = ss.str();
			flags = WebSocket::FRAME_OP_TEXT + 128;
			try
			{
				n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
				PingTimer = 0;
			}
			catch (...)
			{
				err = true;
			}
		}
	}
	break;
	case 2: // cid
	{
		cid = m_tr4->GetCID(ChNumber);
		ss.str("");
		ss.clear();
		if (cid != lastcid[ChNumber])
		{
			ss = CreateJSON("CID", ChNumber, (char*)cid.c_str(),0);
			alpha = ss.str();
			n = ss.str().length();
			lastcid[ChNumber] = cid;
			flags = WebSocket::FRAME_OP_TEXT + 128;
			try
			{
				n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
				PingTimer = 0;
			}
			catch (...)
			{
				err = true;
			}
		}
	}
	break;
	}
}

int WebSocketRequestHandler::ProcessReceive(WebSocket ws)
{
	int ch;
	if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_TEXT)
	{
		Parser		parser;
		buffer[n] = 0;
		std::string stemp = buffer;
		//OutputDebugStringA(buffer);
		Var result;
		try
		{
			result = parser.parse(stemp);
			Object::Ptr	object = result.extract<Object::Ptr>();

			if (result.type() == typeid(Object::Ptr))
			{
				Var	live = object->get("LivePlay");
				Var stop = object->get("LiveStop");
				if (live.isNumeric() && live.isInteger())
				{
					m_LiveChannel = live;
					switch (m_LiveChannel)
					{
					case 1:
						ch = 0;
					break;
					case 2:
						ch = 1;
					break;
					case 4:
						ch = 2;
					break;
					case 8:
						ch = 3;
					break;
					}
					m_rQ[ch]->EmptyQueue();
				}
				else if (stop.isNumeric() && stop.isInteger())
				{
					if (m_LiveChannel == stop)
					{
						m_LiveChannel = 0;
						m_time = 1;
						idx = 0;
					}
				}
			}
		}

		catch ( JSONException& jsone )
		{
			std::cout << jsone.message() << std::endl;
		}
		catch (...)
		{
			// do nothing.
		}

		//ws.sendFrame(buffer, n, flags);
	}
	if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_CLOSE)
	{
		if (!ShutdownSent)
		{
			ws.shutdown();
			ShutdownSent = true;
		}
		return BREAK;
	}
	if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_PING)
	{
		ws.sendFrame(buffer, n, WebSocket::FRAME_OP_PONG);
		return CONTINUE;

	}
	if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_PONG)
	{
		//continue;
		//app.logger().information(Poco::format("PONG received (length=%d, flags=0x%x).", n, unsigned(flags)));
	}
	return SUCCESS;
}


void WebSocketRequestHandler::ProcessSendLiveVoice(WebSocket ws)
{
	std::ostringstream		ss;
	//double					pi = 3.141592653589793238462643383279502884;
	int						ch;
	std::string				base64;
	std::string				alpha;

	const unsigned char*	cbuf = (const unsigned char*)sbuf;
	if (m_LiveChannel)
	{
		switch (m_LiveChannel)
		{
			case 1:
				ch = 0;
			break;
			case 2:
				ch = 1;
			break;
			case 4:
				ch = 2;
			break;
			case 8:
				ch = 3;
			break;
		}
		if (m_rQ[ch]->GetQSize() >= 2000)
		{
			using namespace std::chrono;
			milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			char bfr[256];
			sprintf(bfr,"Here in %d. \n",ms);
			OutputDebugStringA(bfr);
			m_rQ[ch]->Remove(sbuf, 2000);
			base64 = base64_encode(cbuf, 4000);
			ss = CreateJSON("ONLINE", m_LiveChannel, (char*)base64.c_str(), m_time++);
			alpha = ss.str();
			flags = WebSocket::FRAME_OP_TEXT + 128;
			try
			{
				n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
				PingTimer = 0;
			}
			catch (...)
			{
				err = true;
			}
		}

		/*
		m_LiveCounter++;
		if (m_LiveCounter == 5)
		{
			m_LiveCounter = 0;
			for (int i = 0; i < 2000; i++)
			{
				sbuf[i] = (short)(15000 * (double)sin(((double)2 * pi * 440 * idx) / 8000));
				idx++;
				idx %= 8000;
			}
			base64 = base64_encode(cbuf, 4000);
			ss = CreateJSON("ONLINE", m_LiveChannel, (char*)base64.c_str(),m_time++);
			alpha = ss.str();
			flags = WebSocket::FRAME_OP_TEXT + 128;
			try
			{
				n = ws.sendFrame(alpha.c_str(), alpha.length(), flags);
				PingTimer = 0;
			}
			catch (...)
			{
				err = true;
			}
		}
		*/
	}
}