/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpResponse.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/05 06:58:04 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPRESPONSE_H
# define HTTPRESPONSE_H

#include "defines.hpp"
#include "HttpRequest.hpp"
#include "Header.hpp"

class HttpResponse {

	public:
		HttpResponse(void);
		HttpResponse(const HttpRequest& request);
	 	HttpResponse(const HttpResponse& origin);
	 	const HttpResponse& operator=(const HttpResponse& rhs);
	 	~HttpResponse(void);

		Header*				headers;
		
		// ============= Getters ================
		int					getStatusCode() const;
		std::string			getBody() const;
		
		// ============= Setters ================
		void				setProtocol(const std::string& protocol);
		void				setStatusCode(int status);
		void				reSetBody(const std::string& filePath, bool isBinary); //why reset?
		void				fillStandardHeaders(void); //todo: get from config

		// =========== Other ===============
		std::string			serializeResponse(void);
		
	private:
	 	std::string			_protocol;
	 	int 				_statusCode;
	 	std::string			_body;
};

#endif
