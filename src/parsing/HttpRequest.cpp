/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 15:09:34 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <iostream>
#include <sstream>
#include <exception>
#include <algorithm>


HttpRequest::HttpRequest() : _method(), _protocol(), _headers(), _body(), _uri() {};

HttpRequest::HttpRequest(const std::string& request) : _uri() {

// 1. === parse request line === 

	std::size_t RequestLineEnd = request.find("\r\n");
	if (RequestLineEnd == std::string::npos)
		throw std::runtime_error("HttpRequest: first line error");
	
	std::string RequestLine = request.substr(0, RequestLineEnd);
	std::stringstream RequestLineStream(RequestLine);

	std::string tempUriString;
	RequestLineStream >> _method >> tempUriString >> _protocol;
	
	//check protocol:
	if (_protocol != HTTP_PROTOCOL) {
		//505 HTTP Version Not Supported"
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
	// 405 Method Not Allowed
	}
	
	_uri = Uri(tempUriString);
	

// 2. === parse headers ===

	//define block of all headers
	std::size_t headersStart = RequestLineEnd + 2;
	std::size_t headersEnd = request.find("\r\n\r\n", headersStart);
	if (headersEnd == std::string::npos)
		throw std::runtime_error("HttpRequest: headers error");
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

	this->_body = request.substr(headersEnd + 4);
}


HttpRequest::HttpRequest(const HttpRequest& origin) {
	this->_method		= origin._method;
	this->_uri			= origin._uri;
	this->_protocol		= origin._protocol;
	this->_headers		= origin._headers;
	this->_body			= origin._body;

};

const HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs) {
		this->_method		= rhs._method;
		this->_uri			= rhs._uri;
		this->_protocol		= rhs._protocol;
		this->_headers.clear();
		this->_headers		= rhs._headers;
		this->_body			= rhs._body;
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

