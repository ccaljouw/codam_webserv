/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:27:35 by carlo             #+#    #+#             */
/*   Updated: 2023/11/06 12:51:03 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "Uri.hpp"
#include "webServ.hpp"

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