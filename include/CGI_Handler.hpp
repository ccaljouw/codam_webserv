/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:06:07 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 17:34:19 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "Uri.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "eventloop.hpp"

class CGI
{
	public:
		~CGI();
		CGI() = delete;
		CGI(const CGI& other) = delete;
		CGI& operator=(const CGI& other) = delete;
		
		CGI(int epollFd, connection *conn);
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

int cgiHandler(const Uri& uri, connection *conn, int epollFd);

#endif
