/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 16:35:16 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <iostream>
#include <sstream>
#include <exception>
#include <algorithm>


HttpRequest::HttpRequest() : _method(), _protocol(), _headers(), _body(), _uri(), _requestStatus(200) {};

HttpRequest::HttpRequest(const std::string& request) : _uri(), _requestStatus(200) {

// 1. === parse request line === 
try {
	std::size_t RequestLineEnd = request.find("\r\n");
	if (RequestLineEnd == std::string::npos)
		throw parsingException(405, "Bad request");
		
	std::string RequestLine = request.substr(0, RequestLineEnd);
	std::stringstream RequestLineStream(RequestLine);

	std::string tempUriString;
	RequestLineStream >> _method >> tempUriString >> _protocol;
	
	//check protocol
	if (_protocol != HTTP_PROTOCOL) {
		throw parsingException(505, "Version not supported");
	}

	// check method
	bool match = false;
	for (const auto& target : this->supportedMethods) {
		if (target == _method) {
			match = true;
			break;
		}
	}
	if (match == false) {
		throw parsingException(405, "Method not Allowed");
	}
	
	_uri = Uri(tempUriString);
	

// 2. === parse headers ===

	//define block of all headers
	std::size_t headersStart = RequestLineEnd + 2;
	std::size_t headersEnd = request.find("\r\n\r\n", headersStart);
	if (headersEnd == std::string::npos)
		throw parsingException(405, "Bad request");

	std::string HeaderBlock = request.substr(headersStart, headersEnd - headersStart);

	//split header block into seperate lines using a stream and seperating the key value pairs
	std::stringstream HeaderStream(HeaderBlock);
	std::string headerLine;
	while(std::getline(HeaderStream, headerLine)) {
		std::size_t columPos = headerLine.find(":");
		if (columPos != std::string::npos) {
			std::string key = headerLine.substr(0,columPos);
			std::string value = headerLine.substr(columPos + 1);

			//header keys are case insensitive
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				
			//trim leading and trailing whitespaces from header value (not key)
			value = value.substr(key.find_first_not_of(WHITE_SPACE));
			value = value.substr(0, value.find_last_not_of(WHITE_SPACE) + 1);
			_headers.insert(std::make_pair(key, value));
		}
	}

// 3. === parse body ===
	_body = request.substr(headersEnd + 4);
	
	}
	//catch errors	
	catch (const parsingException& exception) {
		_requestStatus = exception.getErrorCode();
		std::cout << exception.what() << std::endl;
	}
}


HttpRequest::HttpRequest(const HttpRequest& origin) {
	_method				= origin._method;
	_uri				= origin._uri;
	_protocol			= origin._protocol;
	_headers			= origin._headers;
	_body				= origin._body;
	_requestStatus		= origin._requestStatus;
};

const HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs) {
		_method			= rhs._method;
		_uri			= rhs._uri;
		_protocol		= rhs._protocol;
		_headers.clear();
		_headers		= rhs._headers;
		_body			= rhs._body;
		_requestStatus	= rhs._requestStatus;
	}
	return *this;
}

HttpRequest::~HttpRequest(void) {
	_headers.clear();
}


//========= Getters ============
std::string HttpRequest::getMethod(void) const								{	return _method;				}
std::string HttpRequest::getProtocol(void) const							{	return _protocol;			}
std::string HttpRequest::getBody(void) const								{	return _body;				}
std::string HttpRequest::getUri(void)										{	return _uri.serializeUri();	}
std::multimap<std::string, std::string>	HttpRequest::getHeaders(void) const	{	return _headers; 			}
int HttpRequest::getRequestStatus(void) const								{	return _requestStatus;		}

