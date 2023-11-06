/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpRequest.h                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 10:27:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 12:07:35 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//todo: remove event stats, check method, protocol, some headers etc during parsing


#pragma once
#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "Uri.h"
#include "webServ.h"

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
		int											getEventStatus(void) const;
		std::multimap<std::string, std::string>		getHeaders(void) const;
	
		
	// ============ Setter ===================
		void			setEventStatus(int status);
	

		private:
		std::string								_method;
		std::string								_protocol;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
		Uri										_uri;
		int										_eventStatus;
};

#endif