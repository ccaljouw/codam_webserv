/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/28 13:48:21 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPRESPONSE_H
# define HTTPRESPONSE_H

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
		
		// ============= Getters ================
		int		getStatusCode() const;
		
		// ============= Setters ================
		void	setProtocol(const std::string& protocol);
		void	setStatusCode(int status);
		void	addHeader(const std::string& key, const std::string& value);
		void	setBody(const std::string& filePath, bool isBinary);
		void	setHeader(const std::string& key, const std::string& value);
		void	fillStandardHeaders(void); //todo: get from config
		void	updateDateLength(void);

		// =========== Other ===============
		std::string	serializeResponse(void);
		std::string	serializeHeaders(void);
		
	private:
	 	std::string								_protocol;
	 	int 									_statusCode;
	 	std::map<std::string, std::string>		_headerMap;
	 	std::string								_body;
};

#endif
