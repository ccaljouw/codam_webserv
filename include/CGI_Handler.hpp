/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:06:07 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 16:33:42 by bfranco       ########   odam.nl         */
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
		
		CGI(connection *con, int epollFd);
		int		getFdIn() const;
		int		getFdOut() const;
		void	closeFds() const;
		int		getStatus() const;
		
	private:
		int		_fdIn;
		int		_fdOut;
		int		_epollFd;
		int		_status;
};

int cgiHandler(const Uri& uri, connection *conn, int epollFd);

#endif
