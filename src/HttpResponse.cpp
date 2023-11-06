/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/01 14:21:11 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/03 16:09:43 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include	"HttpResponse.h"
#include	"webServ.h"	

#include <iostream>
//#include <exception>


HttpResponse::HttpResponse(void) : _protocol(HTTP_PROTOCOL), _statusCode(200), _headers(), _body() {
	//add standard headers status code, 
}

// HttpResponse::HttpResponse(const HttpRequest& request) {

// }


HttpResponse::HttpResponse(const HttpResponse& origin) {
		this->_protocol		= origin._protocol;
		this->_statusCode	= origin._statusCode;
		this->_headers		= origin._headers;
		this->_body			= origin._body;
}

const HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		this->_protocol		= rhs._protocol;
		this->_statusCode	= rhs._statusCode;
		this->_headers.clear();
		this->_headers		= rhs._headers;
		this->_body			= rhs._body;
	}
	return *this;
}

HttpResponse::~HttpResponse(void) {
	_headers.clear();
}

// ============= Setters ================
void HttpResponse::setProtocol(const std::string& protocol)						{ _protocol 	= protocol;	}
void HttpResponse::setStatusCode(int status) 									{ _statusCode	= status;	}
void HttpResponse::setBody(const std::string& body)								{ _body			= body;	}
void HttpResponse::addHeader(const std::string& key, const std::string& value)	{
	_headers.insert(std::make_pair(key, value));
}

// ============= Getters ================
int HttpResponse::getStatusCode() const {
	return this->_statusCode;
}


std::string HttpResponse::serializeHeaders() {
	std::string serializedHeaders;
	for(const auto& headerPair : _headers) {
		serializedHeaders += headerPair.first + ": " + headerPair.second + LINE_END;
	}
	return serializedHeaders;
}


std::string HttpResponse::serializeResponse(void) {
	std::string serializedResponse;

	serializedResponse += _protocol + " " + std::to_string(_statusCode) + LINE_END + serializeHeaders() + LINE_END + _body;
	return serializedResponse;
}

//status code to map
//checks before sending (status code set, stc)
//check WHITE_SPACE
//check capitalseensitivity
//check duplicate headers
//check valid status code
//add content type
//add content_lenght
//add data