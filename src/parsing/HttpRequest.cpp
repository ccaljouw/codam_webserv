/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/29 12:18:11 by carlo         ########   odam.nl         */
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

//  === parse request line === 
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

	
//  === parse headers ===
	//define block of all headers
	std::size_t headersStart = RequestLineEnd + 2;
	std::size_t headersEnd = request.find("\r\n\r\n", headersStart);
	if (headersEnd == std::string::npos)
		throw parsingException(405, "Bad request");

	std::string HeaderBlock = request.substr(headersStart, headersEnd - headersStart);

	//split header block into seperate lines using a stream and seperating the key value pairs
	std::istringstream HeaderStream(HeaderBlock);
	std::string headerLine;
	
	while(std::getline(HeaderStream, headerLine)) {
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
	// test print of headers
	// for (auto& p : _headers)
	// 	std::cout << "key: " << p.first << " | value: " << p.second << std::endl;

// === parse body ===
	_body = request.substr(headersEnd + 4);

// === set uri object === 
	uri = Uri(tempUriString);

// === fetch location specific config settings === 

	_settings = _server->get_locationSettings(getHeaderValue("host"), uri.getPath());
	if (_settings == nullptr)
		throw parsingException(500, "Bad server settings");

	fillStandardHeaders();

	// check allowed method
	if (_settings->_allowedMethods.find(_method) == _settings->_allowedMethods.end())
		throw parsingException(405, "Method not Allowed");


// === set environ vars ===
	std::string host = getHeaderValue("host");
	addEnvironVar("REQUEST_METHOD", getMethod());
	addEnvironVar("QUERY_STRING", getQueryString());
	addEnvironVar("REMOTE_HOST", host);
	addEnvironVar("BODY", getBody());
	addEnvironVar("PATH_INFO", _server->get_rootFolder(host));
	addEnvironVar("UPLOAD_DIR", _server->get_uploadDir(host));
	std::cout << "upload: " << _server->get_uploadDir(host) << std:: endl;


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
const Server* HttpRequest::getServer(void) const 							{	return _server;								}
bool		HttpRequest::getDirListing() const								{	return (this->_settings->_dirListing);		}
std::string	HttpRequest::getRoot(void) const								{	return (this->_settings->_locationRoot);	}
std::string	HttpRequest::getIndex(void) const								{	return (this->_settings->_index);			}
std::set<std::string>	HttpRequest::getAllowedMethods(void) const			{	return (this->_settings->_allowedMethods);	}
std::string	HttpRequest::getLocationId(void) const							{	return (this->_settings->_locationId);		}
std::map<int, std::string>	HttpRequest:: getRedirect(void) const			{	return (this->_settings->_redirect);		}
std::string	HttpRequest::getMethod(void) const								{	return _method;								}
std::string	HttpRequest::getProtocol(void) const							{	return _protocol;							}
std::string	HttpRequest::getBody(void) const								{	return _body;								}
std::string	HttpRequest::getUri(void)										{	return uri.serializeUri();					}
int	HttpRequest::getRequestStatus(void) const								{	return _requestStatus;						}
std::map<std::string, std::string>	HttpRequest::getHeaders(void) const		{	return _headers; 							}

std::string HttpRequest::getHeaderValue(std::string key) const {
	for (const auto& headerPair : _headers) {
		if (headerPair.first == key)
			return headerPair.second;
	}
	return "";
}

std::vector<char*>	HttpRequest::getHeaderVector(void) const {
		
	// make c_string array from headers. first a vector of c_strings, then make pairs and capitalize
	std::vector<char*> c_strings;
	
	for (const auto& pair : _headers) {
		std::string key = pair.first;
		std::string value = pair.second;
		
		for (char& c : key) {	
			c = toupper(static_cast<unsigned char>(c)); 
			if (c == '-')
				c = '_';
		}
		for (char& c : value) {	
			if (c == ',')
				c = ';';
		}
		std::string line = key + "=" + value;
		c_strings.push_back(strdup(line.c_str()));
	}
	c_strings.push_back(nullptr);
	return c_strings;
}


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
	
	for (const auto& pair : _environVars) {
		std::string keyValue = pair.first + "=" + pair.second;
		char* envString = new char[keyValue.length() + 1];
		std::strcpy(envString, keyValue.c_str());
		envString[keyValue.length()] = '\0';
		envArray.push_back(envString);
	}

	//add headers to array
	std::vector<char*> headers = getHeaderVector();
	for (const auto& pair : headers)
		envArray.push_back(pair);
	
	char** result = new char*[envArray.size() + 1];
	size_t index = 0;

	for (const auto& string : envArray)
		result[index++] = string;

	result[index] = nullptr;

	return result;
}

bool	HttpRequest::isHeader(std::string key) {
	auto it = _headers.find(key);
	if (it != _headers.end())
		return true;
	return false;
}


//========= Setters ===============================

void	HttpRequest::setMethod(const std::string& method) 			{	_method = method;			}
void	HttpRequest::setProtocol(const std::string& protocol) 		{	_protocol = protocol;		}
void	HttpRequest::setBody(const std::string& body) 				{	_body = body; 				}
void	HttpRequest::setUri(const std::string& string) 				{	uri = Uri(string);			}		
void	HttpRequest::setRequestStatus(int value) 					{	_requestStatus = value;		}

void	HttpRequest::addHeader(const std::string& key, const std::string& value) {
	if(!value.empty())
		_headers[key] = value;
}

void HttpRequest::setHeader(const std::string& key, const std::string& value) {
	for (auto& headerPair : _headers) {
		if (key == headerPair.first) {
			headerPair.second = value;
			return;
		}
	}
	_headers[key] = value;
}


void	HttpRequest::addEnvironVar(const std::string& key, const std::string& value) {
	if(!value.empty())
		_environVars[key] = value;
}

//todo get from config
void	HttpRequest::fillStandardHeaders() {

	std::string timeout = std::to_string(_server->get_timeout());
	addHeader("Keep-Alive", "timeout=" + timeout + ", max=3");
	addHeader("Date", getTimeStamp());
	addHeader("Server", _server->get_serverName(uri.getHost()));
	addHeader("Last-Modified", getTimeStamp());
	addHeader("Content-Length", std::to_string(_body.length()));
}
