/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.h                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/03 16:08:25 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "Uri.h"

#include <string>
#include <map>

#define LINE_END "\r\n"
#define WHITE_SPACE " \t\n\v\f\r"

class HttpRequest {

	public:
		HttpRequest(void);
		HttpRequest(const std::string& request);
		HttpRequest(const HttpRequest& origin);
		const HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest(void);

	// ============= Getters ================
		std::string		getMethod(void) const;
		std::string		getProtocol(void) const;
		std::string		serializeHeaders(void) const;
		std::string		getBody(void) const;
		std::string		getUri(void);
	
	private:
		std::string								_method;
		std::string								_protocol;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
		Uri										_uri;
};

#endif