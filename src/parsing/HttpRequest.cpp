/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/17 09:00:54 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webServ.hpp"
#include "eventloop.hpp"
#include "Config.hpp"

#include <algorithm>




HttpRequest::HttpRequest(const Server *server) : uri(), _method(), _protocol(), _headers(), _body(), _requestStatus(200), _server(server) {};

HttpRequest::HttpRequest(const std::string& request, const Server *server) : uri(), _requestStatus(200) ,_server(server) {

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
	
	// todo: get location and resulting location settings
	struct LocationSettings location = server->get_locationSettings("servername", "");
	
	// check method
	if (location._allowedMethods.find(_method) == location._allowedMethods.end())
		throw parsingException(405, "Method not Allowed");
	
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
			_headers.insert(std::make_pair(key, value));
		}
	}
	
// 3. === parse body ===
	_body = request.substr(headersEnd + 4);


	}
	//catch block	
	catch (const parsingException& exception)
	{
		_requestStatus = exception.getErrorCode();
		std::cerr << exception.what() << std::endl; 
	}

//fetch and set config file values in request
	setConfigValues(uri.getHost());
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
std::multimap<std::string, std::string>	HttpRequest::getHeaders(void) const		{	return _headers; 			}
int	HttpRequest::getRequestStatus(void) const									{	return _requestStatus;		}

std::string HttpRequest::getHeaderValue(std::string key) const {
	for (const auto& headerPair : _headers) {
		if (headerPair.first == key)
			return headerPair.second;
	}
	return "";
}



char**		HttpRequest::getEnvArray(void) {

	addHeader("REQUEST_METHOD", getMethod());

	// merge headers map and queries map into one map
	std::multimap<std::string, std::string> mergedMap;
	for (const auto& headerPair : _headers)
		mergedMap.insert(headerPair);
	
	for (const auto& queryPair : uri.getQueryMap())
		mergedMap.insert(queryPair);
	
	
	// make c_string array from multimap. first a vector of c_strings.	
	//then make pair and capitalize
	std::vector<char*> c_strings;
	
	for (auto& pair : mergedMap)
	{
		std::string line = pair.first + "=" + pair.second;
		for (char& c : line)
		{	
			c = toupper(static_cast<unsigned char>(c)); //todo: check if this is required and doesnt break stuff
			if (c == '-')
				c = '_';
		}
		c_strings.push_back(strdup(line.c_str()));
	}

	
	c_strings.push_back(strdup(("BODY=" + getBody()).c_str())); // should parse form data in stead?
	c_strings.push_back(nullptr);
	
	//malloc an array and copy vector into array
	char **envArray = new char*[c_strings.size()];
	std::copy(c_strings.begin(), c_strings.end(), envArray);
	
	int k = c_strings.size();
	for (int i = 0; i < k;  i++) 
		std::cout << envArray[i] << std::endl;

	return envArray;
}


//========= Setters ===============================

void	HttpRequest::setMethod(const std::string& method) 			{	_method = method;			}
void	HttpRequest::setProtocol(const std::string& protocol) 		{	_protocol = protocol;		}
void	HttpRequest::setBody(const std::string& body) 				{	_body = body; 				}
void	HttpRequest::setUri(const std::string& string) 				{	uri = Uri(string);			}		
void	HttpRequest::setRequestStatus(int value) 					{	_requestStatus = value;		}

void	HttpRequest::addHeader(const std::string& key, const std::string& value) {
	_headers.insert(std::make_pair(key, value));
}

void	HttpRequest::setConfigValues(std::string host) {
	
	
	// add environ .. seperate environ map
	addHeader("SERVER_NAME", _server->get_serverName(host));
	//add rest
	//CONTENT_TYPE; (including boundry ...)
	//QUERY_STRING;
	//CONTENT_LENGTH
}
