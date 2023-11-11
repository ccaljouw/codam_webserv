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

// #include "HttpResponse.hpp"
// #include "HttpRequest.hpp"
#include "webServ.hpp"

#include <iostream>
#include <fstream>

//declaration of helper functions
std::string getTimeStamp();


HttpResponse::HttpResponse() : _protocol(HTTP_PROTOCOL), _statusCode(200), _headerMap(), _body() {
	fillStandardHeaders();
}


HttpResponse::HttpResponse(const HttpRequest& request) {
	_protocol 			= HTTP_PROTOCOL;
	_statusCode			= request.getRequestStatus();
	_body				= request.getBody() + LINE_END;

	fillStandardHeaders();
}


HttpResponse::HttpResponse(const HttpResponse& origin) {
	*this = origin;
}


const HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		_protocol		= rhs._protocol;
		_statusCode		= rhs._statusCode;
		_headerMap.clear();
		_headerMap		= rhs._headerMap;
		_body			= rhs._body;
	}
	setHeader("Last-Modified", getTimeStamp());
	return *this;
}


HttpResponse::~HttpResponse() {
	_headerMap.clear();
}


// ============= Getters ================
int HttpResponse::getStatusCode() const {	return _statusCode;	}


// ============= Setters ================
void HttpResponse::setProtocol(const std::string& protocol)	{ 
	_protocol 	= 	protocol;

	setHeader("Last-Modified", getTimeStamp());
}


void HttpResponse::setStatusCode(int status) {
	_statusCode	= status;

	setHeader("Last-Modified", getTimeStamp());
}


void HttpResponse::addHeader(const std::string& key, const std::string& value) {
	_headerMap.insert(std::make_pair(key, value));

	setHeader("Last-Modified", getTimeStamp());
}


void HttpResponse::setHeader(const std::string& key, const std::string& value) {
	for (auto& headerPair : _headerMap) {
		if (key == headerPair.first) {
			headerPair.second = value;
			return;
		}
	}
	_headerMap.insert(std::make_pair(key, value));
}


void HttpResponse::setBody(const std::string& filePath)	{
	std::ifstream inputFile(filePath);
	if (inputFile.is_open()) {

		std::string line;
		while(std::getline(inputFile, line)) {
			_body += line;
			_body += LINE_END;
		}
		inputFile.close();

	} else { 
		throw HttpRequest::parsingException(422, "Unprocessable Entity");
	}
	size_t bodyLength = _body.length();

	setHeader("Content-Length", std::to_string(bodyLength));
	setHeader("Last-Modified", getTimeStamp());
}


void HttpResponse::fillStandardHeaders() {
	// addHeader("Connection", "keep-alive:);
	// addHeader("Cache-Control",  "max-age=0");
	// addHeader("Keep-Alive", "timeout=5, max=997");
	// addHeader("Set-Cookie", "...");
	// addHeader("Transfer-Encoding", "chunked");
	addHeader("Date", getTimeStamp());
	addHeader("Content-type", "text/html; charset=utf-8");
	addHeader("Server", "CODAM_WEBSERV");
	
	size_t bodyLength = _body.length();
	setHeader("Content-Length", std::to_string(bodyLength));
	setHeader("Last-Modified", getTimeStamp());
}


//===== Other =============

std::string HttpResponse::serializeHeaders() {
	std::string serializedHeaders;
	for(const auto& headerPair : _headerMap) {
		serializedHeaders += headerPair.first + ": " + headerPair.second + LINE_END;
	}
	return serializedHeaders;
}


std::string HttpResponse::serializeResponse() {
	std::string serializedResponse;
	serializedResponse += _protocol + " " + std::to_string(_statusCode) + LINE_END + serializeHeaders() + LINE_END + _body + LINE_END;
	return serializedResponse;
}



//todo:
//check method protocol, some headers
//checks before sending (status code set, stc)
//check WHITE_SPACE
//check capitalseensitivity
//check duplicate headers
//check valid status code
//add content type
//add content_lenght
//add data
//add set_last_modified to all set functions