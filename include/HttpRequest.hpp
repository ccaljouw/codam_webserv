/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 16:27:25 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "webServ.hpp"
#include "Uri.hpp"

#include <string>
#include <map>

class HttpRequest {

	public:
		HttpRequest(void);
		HttpRequest(const std::string& request);
		HttpRequest(const HttpRequest& origin);
		const HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest(void);

	// ============= Getters ================
		std::string									getMethod(void) const;
		std::string									getProtocol(void) const;
		std::string									getBody(void) const;
		std::string									getUri(void);
		std::multimap<std::string, std::string>		getHeaders(void) const;
		int											getRequestStatus(void) const;

		class parsingException : public std::exception {
			public:
				parsingException(int errorCode, const std::string& message) : _errorCode(errorCode), _message(message) {}
				const char*	what() const noexcept override		{	return _message.c_str();	}
				int			getErrorCode() const 				{	return _errorCode;			}

			private:
				int				_errorCode;
				std::string		_message;
		};
	
		
	private:
		std::string								_method;
		std::string								_protocol;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
		Uri										_uri;
		int										_requestStatus;

		std::vector<std::string> supportedMethods = { "GET", "POST", "DELETE" };
};

#endif