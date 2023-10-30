#pragma once


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco\Net\NameValueCollection.h"
#include "poco/net/HTTPCookie.h"
#include "Poco/net/WebSocket.h"
#include <iostream>
#include <vector>

#include <iostream> 
#include <fstream>  

using Poco::Net::WebSocket;
using Poco::Net::MessageHeader;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::NameValueCollection;
using Poco::Net::PartHandler;
using Poco::Net::HTTPRequest;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPCookie;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

class CDb;
class CClientHandler : public HTTPRequestHandler
{
public:
	CClientHandler(std::string format);
	~CClientHandler();
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
	void SendDefaultPage(HTTPServerResponse& response);
	void ProcessLogin(HTTPServerRequest& request, HTTPServerResponse& response);
	//void ProcessCallList(HTTPServerRequest& request, HTTPServerResponse& response);
	void ProcessDefaultError(HTTPServerRequest& request, HTTPServerResponse& response);
	void ProcessDefault(HTTPServerRequest& request, HTTPServerResponse& response);
	void ProcessOtherReqs(HTTPServerRequest& request, HTTPServerResponse& response);
	void SendChannelHTML(HTTPServerRequest& request, HTTPServerResponse& response);
	void ProcessLogout(HTTPServerRequest& request, HTTPServerResponse& response);
private:
	std::string		_format;
	std::string		 m_path;
	//CDb*			 m_db;
};

