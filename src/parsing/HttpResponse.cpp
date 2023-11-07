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

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webServ.hpp"

#include <iostream>
#include <fstream>

//declaration of helper functions
std::string getTimeStamp();


HttpResponse::HttpResponse(void) : _protocol(HTTP_PROTOCOL), _statusCode(200), _headers(), _body() {
	fillStandardHeaders();
}


HttpResponse::HttpResponse(const HttpRequest& request) {
	_protocol 			= HTTP_PROTOCOL;
	_statusCode			= request.getRequestStatus();
	_body				= request.getBody() + LINE_END;

	fillStandardHeaders();
}


HttpResponse::HttpResponse(const HttpResponse& origin) {
	_protocol			= origin._protocol;
	_statusCode			= origin._statusCode;
	_headers			= origin._headers;
	_body				= origin._body;

	fillStandardHeaders();
}


const HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		_protocol		= rhs._protocol;
		_statusCode		= rhs._statusCode;
		_headers.clear();
		_headers		= rhs._headers;
		_body			= rhs._body;
	}
	updateDateLength();
	return *this;
}


HttpResponse::~HttpResponse(void) {
	_headers.clear();
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
	serializedResponse += _protocol + " " + std::to_string(_statusCode) + LINE_END + serializeHeaders() + LINE_END + _body + LINE_END;
	return serializedResponse;
}


// ============= Getters ================
int HttpResponse::getStatusCode() const {	return _statusCode;	}


// ============= Setters ================
void HttpResponse::setProtocol(const std::string& protocol)	{ 
	_protocol 	= 	protocol;

	updateDateLength();
}


void HttpResponse::setStatusCode(int status) {
	_statusCode	= status;

	updateDateLength();
}


void HttpResponse::addHeader(const std::string& key, const std::string& value) {
	_headers.insert(std::make_pair(key, value));

	updateDateLength();
}


void HttpResponse::setHeader(const std::string& key, const std::string& value) {
	for (auto& headerPair : _headers) {
		if (key == headerPair.first) {
			headerPair.second = value;
			return;
		}
	}
	_headers.insert(std::make_pair(key, value));
	setHeader("Content-Length", std::to_string(serializeResponse().size()));
}


void HttpResponse::setBody(const std::string& filePath)	{
	std::ifstream inputFile(filePath);
	if (inputFile.is_open()) {

		std::string line;
		while(std::getline(inputFile, line)) {
			_body += line;
			_body += '\n';
		}
		inputFile.close();

	} else { 
		throw HttpRequest::parsingException(422, "Unprocessable Entity");
	}
	updateDateLength();
}


void HttpResponse::fillStandardHeaders() {
	// addHeader("Connection", "keep-alive:);
	// addHeader("Cache-Control",  "max-age=0");
	// addHeader("Keep-Alive", "timeout=5, max=997");
	// addHeader("Set-Cookie", "...");
	// addHeader("Transfer-Encoding", "chunked");
	addHeader("Date", getTimeStamp());
	addHeader("Last-Modified", getTimeStamp());
	addHeader("Content-type", "text/html; charset=utf-8");
	addHeader("Server", "CODAM_WEBSERV");
	updateDateLength();
}

void HttpResponse::updateDateLength(void) {
	setHeader("Last-Modified", getTimeStamp());
	int len = serializeResponse().size();
	setHeader("Content-Length", std::to_string(len));
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