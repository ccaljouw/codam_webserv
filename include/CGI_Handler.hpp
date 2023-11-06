/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI_Handler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 12:06:07 by bfranco           #+#    #+#             */
/*   Updated: 2023/11/06 14:54:40 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "Uri.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class CGI
{
	public:
		CGI() = delete;
		~CGI();
		CGI(const CGI& other) = delete;
		CGI& operator=(const CGI& other) = delete;
		
		CGI(int epollFd);
		int getStatus() const;
		int getFdIn() const;
		int getFdOut() const;
		void	closeFds() const;
		
	private:
		int		_fdIn;
		int		_fdOut;
		int		_epollFd;
		int		_status;
};

void cgiHandler(const Uri& uri, HttpResponse& response, int epollFd);

#endif
