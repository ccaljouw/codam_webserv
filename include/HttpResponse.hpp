/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:27:35 by carlo             #+#    #+#             */
/*   Updated: 2023/11/06 12:56:28 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPRESPONSE_H
# define HTTPRESPONSE_H

#include "Uri.hpp"
#include "webServ.hpp"

#include <string>
#include <map>

class HttpRequest;

class HttpResponse {

	public:
	 	HttpResponse(void);
		HttpResponse(const HttpRequest& request);
	 	HttpResponse(const HttpResponse& origin);
	 	const HttpResponse& operator=(const HttpResponse& rhs);
	 	~HttpResponse(void);

	// ============= Setters ================
		void setProtocol(const std::string& protocol);
		void setStatusCode(int status);
		void addHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);

	// ============= Getters ================
		int getStatusCode() const;
		
		std::string serializeHeaders(void);
		std::string serializeResponse(void);
		
	private:
	 	std::string								_protocol;
	 	int 									_statusCode;
	 	std::multimap<std::string, std::string>	_headers;
	 	std::string								_body;
};

#endif