/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.h                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/03 17:11:50 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPRESPONSE_H
# define HTTPRESPONSE_H

#include "Uri.h"
#include "webServ.h"

#include <string>
#include <map>

class HttpResponse {

	public:
	 	HttpResponse(void);
	 	HttpResponse(const HttpResponse& origin);
	 	const HttpResponse& operator=(const HttpResponse& rhs);
	 	~HttpResponse(void);

	// ============= Setters ================
		void setProtocol(const std::string& protocol);
		void setStatusCode(const int& status);
		void addHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);

		std::string serializeResponse(void);
		
	private:
	 	std::string								_protocol;
	 	int 									_statusCode;
	 	std::multimap<std::string, std::string>	_headers;
	 	std::string								_body;
};

#endif