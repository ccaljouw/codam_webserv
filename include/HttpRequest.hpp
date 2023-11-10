/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/10 15:55:06 by cariencaljo   ########   odam.nl         */
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

		Uri										uri;

		
	// ============= Getters ================
		std::string									getMethod(void) const;
		std::string									getProtocol(void) const;
		std::string									getBody(void) const;
		std::string									getUri(void);
		std::multimap<std::string, std::string>		getHeaders(void) const;
		int											getRequestStatus(void) const;
		char** 										getEnvArray(void) const; //includes header and queries merged into one array
			

	// ============= Setters ================
		void		setMethod(const std::string& method);
		void		setProtocol(const std::string& protocol);
		void		setBody(const std::string& body);
		void		setUri(const std::string& str);
		void		addHeader(const std::string& key, const std::string& value);
		void		setRequestStatus(int value);


	// ============= exception ================
		class parsingException : public std::exception {
			public:
				parsingException(int errorCode, const std::string& message) : _errorCode(errorCode), _message(message) {}
				const char*	what() const noexcept override		{	return _message.c_str();	} //waarom c_str()?
				int			getErrorCode() const 				{	return _errorCode;			}

			private:
				int								_errorCode;
				std::string						_message;
		};
	
		
	private:
		std::string								_method;
		std::string								_protocol;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
		int										_requestStatus;

		std::vector<std::string> supportedMethods = { "GET", "POST", "DELETE" }; //todo: make configurable
};

#endif