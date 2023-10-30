#pragma once


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Array.h"
#include "Poco/JSON/Object.h"
#include "Poco/Format.h"
#include "Poco/Exception.h"
#include "Poco/Dynamic/Struct.h"
#include <set>
#include <sstream>
#include "ShortQReader.h"



#include <iostream>
#include <string>
#include "general.h"

using Poco::Exception;
using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::JSON::Parser;
using Poco::JSON::Array;
using Poco::JSON::Object;

using namespace Poco::JSON;
using namespace Poco::Dynamic;
using Poco::DynamicStruct;


#define  PING_TIME 4096  // 4 seconds

#define		CONTINUE	 2
#define		BREAK		 1
#define		SUCCESS		 0
#define		FAILURE		-1

class WebSocketRequestHandler : public HTTPRequestHandler
{
public:
								 WebSocketRequestHandler();
								~WebSocketRequestHandler();
			std::ostringstream	CreateJSON(char* cmd, int channel, char* param,int time);
			void				handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
			void				ProcessPing(WebSocket ws);
			void				ProcessSignaling(WebSocket ws);
			void				ProcessSendLiveVoice(WebSocket ws);
			int					ProcessReceive(WebSocket ws);
private:
			int					lastState[MAX_CHANNEL_NUMBER];
			int					idx;
			std::string			lastcid[MAX_CHANNEL_NUMBER];
			std::string			lastdial[MAX_CHANNEL_NUMBER];
			u32					m_LiveChannel;
			bool				err;
			int					flags;
			int					PingTimer ;
			int					ChNumber ;
			char				buffer[1024];
			int					n;
			bool				ShutdownSent;
			int					state;
			int					m_LiveCounter;
			int					m_time;
			CShortQReader*		m_rQ[MAX_CHANNEL_NUMBER];
			short				sbuf[2000];
};

