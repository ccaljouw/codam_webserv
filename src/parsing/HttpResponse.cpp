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


std::string getTimeStamp();

HttpResponse::HttpResponse() : _protocol(HTTP_PROTOCOL), _statusCode(200), _headerMap(), _body() {
	fillStandardHeaders();
}


HttpResponse::HttpResponse(const HttpRequest& request) {
	_protocol 			= HTTP_PROTOCOL;
	_statusCode			= request.getRequestStatus();
	_body				= request.getBody();

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


void HttpResponse::setBody(const std::string& filePath, bool isBinary)	{
	std::ifstream inputFile;
	int length;

	if (isBinary)
		inputFile.open(filePath, std::ifstream::binary);
	else
		inputFile.open(filePath);
	if (inputFile.good()) {
		inputFile.seekg(0, inputFile.end);
		length = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		char buffer[length];
		inputFile.read(buffer, length);
		_body.append(buffer, length);

		if (!inputFile)
			std::cout << "could only read " << inputFile.gcount() << " from " << length << " bites" << std::endl; //todo trow error
		
		inputFile.close();
	}
	
	else
		throw HttpRequest::parsingException(422, "Unprocessable Entity");
	
	setHeader("Last-Modified", getTimeStamp());
	setHeader("Content-Length", std::to_string(length));
}


void HttpResponse::fillStandardHeaders() {
	// addHeader("Transfer-Encoding", "chunked");
	// addHeader("Cache-Control",  "public, max-age=86400");
	addHeader("Keep-Alive", "timeout=5, max=3"); // get timout and max requests from server in connection struct
	addHeader("Date", getTimeStamp());
	addHeader("Server", HOST); // get server name from server in connection struct
	addHeader("Last-Modified", getTimeStamp());
	addHeader("Content-Length", std::to_string(_body.length()));
}


//===== Other =============

std::string HttpResponse::serializeHeaders() {
	std::string serializedHeaders;
	for(const auto& headerPair : _headerMap)
		serializedHeaders += headerPair.first + ": " + headerPair.second + LINE_END;
	
	return serializedHeaders;
}


std::string HttpResponse::serializeResponse() {
	std::string serializedResponse;
	serializedResponse += _protocol + " " + std::to_string(_statusCode) + LINE_END + serializeHeaders() + LINE_END + _body + LINE_END;
	return serializedResponse;
}
