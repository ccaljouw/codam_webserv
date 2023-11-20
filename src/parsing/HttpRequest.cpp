/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/20 15:55:38 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webServ.hpp"
#include "eventloop.hpp"
#include "Config.hpp"

#include <algorithm>

std::string getTimeStamp();


HttpRequest::HttpRequest(const Server *server) : uri(), _method(), _protocol(), _headers(), _environVars(), _body(), _requestStatus(200), _server(server), _settings()  {};

HttpRequest::HttpRequest(const std::string& request, const Server *server) : uri(), _requestStatus(200), _server(server)  {


//todo: switch into helper functions mapHeaders
// 1. === parse request line === 
try {
	std::size_t RequestLineEnd = request.find("\r\n");
	if (RequestLineEnd == std::string::npos)
		throw parsingException(405, "Bad request");
		
	std::string RequestLine = request.substr(0, RequestLineEnd);
	
	std::stringstream RequestLineStream(RequestLine);
	std::string tempUriString;

	RequestLineStream >> _method >> tempUriString >> _protocol;
	
	//check protocol //todo test does not work
	if (_protocol != HTTP_PROTOCOL)
		throw parsingException(505, "Version not supported");

	uri = Uri(tempUriString);

	//fetch location specific config settings
	_settings = _server->get_locationSettings(uri.getHost(), uri.getPath());

	// check method
	if (_settings->_allowedMethods.find(_method) == _settings->_allowedMethods.end())
		throw parsingException(405, "Method not Allowed");
	// else
	// 	std::cout << "method ok" << std::endl; //todo remove line
	//todo:add all checks etc
	
// 2. === parse headers ===
	//define block of all headers
	std::size_t headersStart = RequestLineEnd + 2;
	std::size_t headersEnd = request.find("\r\n\r\n", headersStart);
	if (headersEnd == std::string::npos)
		throw parsingException(405, "Bad request");

	std::string HeaderBlock = request.substr(headersStart, headersEnd - headersStart);

	//split header block into seperate lines using a stream and seperating the key value pairs
	std::istringstream HeaderStream(HeaderBlock);
	std::string headerLine;
	
	while(std::getline(HeaderStream, headerLine))
	{
		std::size_t columPos = headerLine.find(":");
		if (columPos != std::string::npos)
		{
			std::string key = headerLine.substr(0,columPos);
			std::string value = headerLine.substr(columPos + 1);

			//header keys are case insensitive
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				
			value = removeWhitespaces(value);
			_headers[key] = value;
		}
	}

	addHeader("Last-Modified", getTimeStamp());
	
	// std::cout << getHeadersString() << std::endl;	//todo: remove

	
// 3. === parse body ===
	_body = request.substr(headersEnd + 4);

// 4. ==== setenvironvars ====
	addEnvironVar("REQUEST_METHOD", getMethod());
	addEnvironVar("CONTENT_TYPE", getHeaderValue("content-type"));
	addEnvironVar("CONTENT_LENGTH", getHeaderValue("content-length"));
	addEnvironVar("QUERY_STRING", getQueryString());
	addEnvironVar("REMOTE_HOST", uri.getHost());




	}
	//catch block	
	catch (const parsingException& exception)
	{
		_requestStatus = exception.getErrorCode();
		std::cerr << exception.what() << std::endl; 
	}
}



HttpRequest::HttpRequest(const HttpRequest& origin) {
	*this = origin;
};


const HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs)
	{
		uri				= rhs.uri;
		_method			= rhs._method;
		_protocol		= rhs._protocol;
		_headers.clear();
		_headers		= rhs._headers;
		_body			= rhs._body;
		_requestStatus	= rhs._requestStatus;
		_server			= rhs._server;
		_settings		= rhs._settings;
	}
	return *this;
}

HttpRequest::~HttpRequest() {
	_headers.clear();
}


//========= Getters ===============================

std::string	HttpRequest::getMethod(void) const									{	return _method;				}
std::string	HttpRequest::getProtocol(void) const								{	return _protocol;			}
std::string	HttpRequest::getBody(void) const									{	return _body;				}
std::string	HttpRequest::getUri(void)											{	return uri.serializeUri();	}
int	HttpRequest::getRequestStatus(void) const									{	return _requestStatus;		}
std::map<std::string, std::string>	HttpRequest::getHeaders(void) const			{	return _headers; 			}


char*	HttpRequest::getHeadersString(void) const {
	std::ostringstream os;
	
	for (const auto& pair : _headers)
		os << pair.first << ":" << pair.second << "\r\n";
	std::string result = os.str();
	
	char* cstring = new char[result.length() + 1];
	std::strcpy(cstring, result.c_str());
	cstring[result.length()] = '\0';
	
	return cstring;
}

char*	HttpRequest::getQueryString(void) const {
	std::ostringstream os;
	
	std::map<std::string, std::string> queries = uri.getQueryMap();
	for (const auto& pair : queries)
		os << pair.first << "=" << pair.second << "&";
	std::string result = os.str();
	if (!result.empty())
		result.pop_back(); //remove last &
	
	char* cstring = new char[result.length() + 1];
	std::strcpy(cstring, result.c_str());
	cstring[result.length()] = '\0';
	
	return cstring;
}

char*	HttpRequest::getEnvString(void) const {
	std::ostringstream os;
	
	for (const auto& pair : _environVars)
		os << pair.first << "=" << pair.second << ";";
	std::string result = os.str();
	
	char* cstring = new char[result.length() + 1];
	std::strcpy(cstring, result.c_str());
	cstring[result.length()] = '\0';
	
	return cstring;
}

char** HttpRequest::getEnvArray(void) const {
	std::vector<char*> envArray;
	
	for (const auto& pair : _environVars) {
		std::string keyValue = pair.first + "=" + pair.second;
		char* envString = new char[keyValue.length() + 1];
		std::strcpy(envString, keyValue.c_str());
		envString[keyValue.length()] = '\0';
		envArray.push_back(envString);
	}
	char** result = new char*[envArray.size() + 1];
	size_t index = 0;

	for (const auto& string : envArray)
		result[index++] = string;


	//todo check requirements
	//add headers
	// result[index++] = getHeadersString();

	result[index] = nullptr;
	return result;
}


std::string HttpRequest::getHeaderValue(std::string key) const {
	for (const auto& headerPair : _headers) {
		if (headerPair.first == key)
			return headerPair.second;
	}
	return "";
}



//========= Setters ===============================

void	HttpRequest::setMethod(const std::string& method) 			{	_method = method;			}
void	HttpRequest::setProtocol(const std::string& protocol) 		{	_protocol = protocol;		}
void	HttpRequest::setBody(const std::string& body) 				{	_body = body; 				}
void	HttpRequest::setUri(const std::string& string) 				{	uri = Uri(string);			}		
void	HttpRequest::setRequestStatus(int value) 					{	_requestStatus = value;		}

void	HttpRequest::addHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void	HttpRequest::addEnvironVar(const std::string& key, const std::string& value) {
	_environVars[key] = value;
}
