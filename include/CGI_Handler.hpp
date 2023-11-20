/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:06:07 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/20 14:58:52 by bfranco       ########   odam.nl         */
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
		CGI() = delete;
		CGI(const CGI& other) = delete;
		CGI& operator=(const CGI& other) = delete;
		
		~CGI();
		CGI(int epollFd, connection *conn);
		int		getStatus() const;
		int		getFdIn() const;
		int		getFdOut() const;
		void	closeFds();
		
	private:
		int		_epollFd;
		int		_status;
		int		_fdIn;
		int		_fdOut;
};

int	cgiHandler(const HttpRequest& req, connection *conn, int epollFd); // todo : const ref request

#endif
