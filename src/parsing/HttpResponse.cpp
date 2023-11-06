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


HttpResponse::HttpResponse(void) : _protocol(HTTP_PROTOCOL), _statusCode(200), _headers(), _body() {
//todo: add standard headers and status code, 
}

HttpResponse::HttpResponse(const HttpRequest& request) {
//todo: add standard headers and status code, 

	_protocol 			= HTTP_PROTOCOL;
	_statusCode			= request.getRequestStatus();
	_body				= "here is the request body: " + request.getBody() + LINE_END;
}

//todo: standard headers: cookie date, length, host, content type, 
HttpResponse::HttpResponse(const HttpResponse& origin) {
	_protocol			= origin._protocol;
	_statusCode			= origin._statusCode;
	_headers			= origin._headers;
	_body				= origin._body;

	addHeader("host", HOST);
	addHeader("date", "temp date");
	addHeader("Content_length", "temp length");
}

const HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		_protocol		= rhs._protocol;
		_statusCode		= rhs._statusCode;
		_headers.clear();
		_headers		= rhs._headers;
		_body			= rhs._body;
	}
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

	serializedResponse += _protocol + " " + std::to_string(_statusCode) + LINE_END + serializeHeaders() + LINE_END + _body;
	return serializedResponse;
}


// ============= Getters ================
int HttpResponse::getStatusCode() const {	return _statusCode;	}


// ============= Setters ================
void HttpResponse::setProtocol(const std::string& protocol)						{ _protocol 	= 	protocol;					}
void HttpResponse::setStatusCode(int status) 									{ _statusCode	= 	status;						}
void HttpResponse::addHeader(const std::string& key, const std::string& value)	{ _headers.insert(std::make_pair(key, value));	}

void HttpResponse::setBody(const std::string& filePath) {
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
}



//todo:
//check method protocol, some headers
//status code to map
//checks before sending (status code set, stc)
//check WHITE_SPACE
//check capitalseensitivity
//check duplicate headers
//check valid status code
//add content type
//add content_lenght
//add data