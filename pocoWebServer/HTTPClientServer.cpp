#include "HTTPClientServer.h"
#include "ClientHandlerFactory.h"
#include "IniHandler.h"


CHTTPClientServer::CHTTPClientServer() : _helpRequested(false)
{

}

CHTTPClientServer::~CHTTPClientServer()
{

}

void CHTTPClientServer::initialize(Application& self)
{
	loadConfiguration();
	ServerApplication::initialize(self);
}

void CHTTPClientServer::uninitialize()
{
	ServerApplication::uninitialize();
}

void CHTTPClientServer::defineOptions(OptionSet& options)
{
	ServerApplication::defineOptions(options);

	options.addOption(
		Option("help", "h", "display argument help information")
		.required(false)
		.repeatable(false)
		.callback(OptionCallback<CHTTPClientServer>(this, &CHTTPClientServer::handleHelp)));
}

void CHTTPClientServer::handleHelp(const std::string& name,const std::string& value)
{
	HelpFormatter helpFormatter(options());
	helpFormatter.setCommand(commandName());
	helpFormatter.setUsage("OPTIONS");
	helpFormatter.setHeader("A web server that serves the current date and time.");
	helpFormatter.format(std::cout);
	stopOptionsProcessing();
	_helpRequested = true;
}

int CHTTPClientServer::main(const std::vector<std::string>& args)
{
	if (!_helpRequested)
	{
		char key[32];
		strcpy(key, "DBWebServerPort");
		char *keyPtr = &key[0];

		char value[32];
		char *valuePtr = &value[0];
		char* defaultPort = {"80"};


		CIniHandler* m_pIni = new CIniHandler(SETTING_FILE_NAME);
		valuePtr = &value[0];
		m_pIni->GetValue("Server", keyPtr, valuePtr, (char*)defaultPort);
		int res = atoi(valuePtr);

		unsigned short port = 2077;// res;
		delete m_pIni;
		std::string format(config().getString("HTTPTimeServer.format",DateTimeFormat::SORTABLE_FORMAT));
		ServerSocket svs(port);
		HTTPServer srv(new CClientHandlerFactory(format),svs, new HTTPServerParams);
		srv.start();
		waitForTerminationRequest();
		srv.stop();
	}
	return Application::EXIT_OK;
}