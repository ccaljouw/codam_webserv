/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/08 20:41:51 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webServ.hpp"

std::string getTimeStamp();

HttpRequest::HttpRequest(const Server *server) : uri(), headers(), _method(), _protocol(), _environVars(), _body(), _requestStatus(200), _server(server), _settings()  {
	headers = new Header();
	fillStandardHeaders();
}

HttpRequest::HttpRequest(const std::string& request, const Server *server) : uri(), headers(), _requestStatus(200), _server(server)  {

	//  === parse request line === 
	try {
		// std::cout << "start request contructor" << std::endl;
		std::size_t RequestLineEnd = request.find("\r\n");
		if (RequestLineEnd == std::string::npos)
			throw parsingException(400, "Bad request");
			
		std::string RequestLine = request.substr(0, RequestLineEnd);
		
		// std::cout << " request contructor" << std::endl;
		std::stringstream RequestLineStream(RequestLine);
		// if (!RequestLineStream.good())
		// 	std::cout << "string stream error" << std::endl;
		std::string tempUriString;
		// std::cout << "before processing stream request contructor" << std::endl;
		RequestLineStream >> _method >> tempUriString >> _protocol;
		// std::cout << "after processing stream request contructor" << std::endl;	
		// === set uri object === 
		uri = Uri(tempUriString);
		
		//check protocol //todo test does not work
		if (_protocol != HTTP_PROTOCOL)
			throw parsingException(505, "Version not supported");

		
	//  === parse headers ===
		//define block of all headers
		std::size_t headersStart = RequestLineEnd + 2;
		std::size_t headersEnd = request.find("\r\n\r\n", headersStart);
		if (headersEnd == std::string::npos)
			throw parsingException(400, "Bad request");

		std::string HeaderBlock = request.substr(headersStart, headersEnd - headersStart);

		headers = new Header(HeaderBlock);

	// === parse body ===
		_body = request.substr(headersEnd + 4);

	// === fetch location specific config settings === 
		_settings = _server->get_locationSettings(headers->getHeaderValue("host"), uri.getPath());
		if (_settings == nullptr)
			throw parsingException(500, "Bad server settings");

		fillStandardHeaders();

		// check allowed method
		if (_settings->_allowedMethods.find(_method) == _settings->_allowedMethods.end())
			throw parsingException(405, "Method not Allowed");

	// === set environ vars ===
		std::string host = headers->getHeaderValue("host");
		std::string location = uri.getPath();
		addEnvironVar("REQUEST_METHOD", getMethod());
		addEnvironVar("QUERY_STRING", getQueryString());
		addEnvironVar("REMOTE_HOST", host);
		addEnvironVar("BODY", getBody());
		addEnvironVar("PATH_INFO", _server->get_rootFolder(host, location));
		addEnvironVar("UPLOAD_DIR", _server->get_uploadDir(host, location));
}
//catch block	
catch (const parsingException& exception) {
	_requestStatus = exception.getErrorCode();
	std::cerr << exception.what() << std::endl; 
}
}

HttpRequest::HttpRequest(const HttpRequest& origin) {
	*this = origin;
};

const HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs) {
		uri				= rhs.uri;
		headers			= rhs.headers;
		_method			= rhs._method;
		_protocol		= rhs._protocol;
		_body			= rhs._body;
		_requestStatus	= rhs._requestStatus;
		_server			= rhs._server;
		_environVars.clear();
		_environVars	= rhs._environVars;
		_settings		= rhs._settings;
		
	}
	return *this;
}

HttpRequest::~HttpRequest() {
	_environVars.clear();
	delete headers;
}


//========= Getters ===============================
const Server*				HttpRequest::getServer(void) const 			{	return _server;								}
std::set<std::string>		HttpRequest::getAllowedMethods(void) const	{	return (this->_settings->_allowedMethods);	}
std::string					HttpRequest::getLocationId(void) const		{	return (this->_settings->_locationId);		}
std::map<int, std::string>	HttpRequest::getRedirect(void) const		{	return (this->_settings->_redirect);		}
std::string					HttpRequest::getMethod(void) const			{	return _method;								}
std::string					HttpRequest::getProtocol(void) const		{	return _protocol;							}
std::string					HttpRequest::getBody(void) const			{	return _body;								}
std::string					HttpRequest::getUri(void)					{	return uri.serializeUri();					}
int							HttpRequest::getRequestStatus(void) const	{	return _requestStatus;						}


std::string	HttpRequest::getQueryString(void) const {
	std::ostringstream os;
	
	std::map<std::string, std::string> queries = uri.getQueryMap();
	for (const auto& pair : queries)
		os << pair.first << "=" << pair.second << "&";
	std::string result = os.str();
	if (!result.empty())
		result.pop_back(); //remove last '&'
	
	return result;
}


//makes and char** array as input for execve
char**	HttpRequest::getEnvArray(void) const {

	std::vector<char*> envArray;
	
	//process  _enrironVars
	for (const auto& pair : _environVars) {
		std::string keyValue = pair.first + "=" + pair.second;
		char* envString = new char[keyValue.length() + 1];
		std::strcpy(envString, keyValue.c_str());
		envArray.push_back(envString);
	}

	//process  headers
	std::vector<char*> headerVector = HttpRequest::headers->getHeaderVector();
	for (const auto& pair : headerVector)
		envArray.push_back(pair);
	
	char** result = new char*[envArray.size() + 1];
	size_t index = 0;

	for (const auto& string : envArray)
		result[index++] = string;

	result[index] = nullptr;
	return result;
}


//========= Setters ===============================

void	HttpRequest::setMethod(const std::string& method) 			{	_method = method;			}
void	HttpRequest::setProtocol(const std::string& protocol) 		{	_protocol = protocol;		}
void	HttpRequest::setBody(const std::string& body) 				{	_body = body; 				}
void	HttpRequest::setUri(const std::string& string) 				{	uri = Uri(string);			}		
void	HttpRequest::setRequestStatus(int value) 					{	_requestStatus = value;		}

void	HttpRequest::addEnvironVar(const std::string& key, const std::string& value) {
	if(!value.empty())
		_environVars[key] = value;
}

void	HttpRequest::fillStandardHeaders() {

	std::string timeout = std::to_string(_server->get_timeout());
	// headers->addHeader("Keep-Alive", "timeout=" + timeout + ", max=3"); //todo from config
	headers->addHeader("Date", getTimeStamp());
	headers->addHeader("Server", _server->get_serverName(uri.getHost()));
	headers->addHeader("Last-Modified", getTimeStamp());
	headers->addHeader("Content-Length", std::to_string(_body.length()));
}
