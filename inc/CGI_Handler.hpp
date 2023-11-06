/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:06:07 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 13:19:11 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "Uri.hpp"
#include "HttpResquest.hpp"

class CGI
{
	public:
		CGI() = delete;
		~CGI() = delete;
		CGI(const CGI& other) = delete;
		CGI& operator=(const CGI& other) = delete;
		
		CGI(int epollFd);
		const int getStatus() const;
		const int getFdIn() const;
		const int getFdOut() const;
		void	closeFds() const;
		
	private:
		int		_fdIn;
		int		_fdOut;
		int		_epollFd;
		int		_status;
};

void cgiHandler(const Uri& uri, HttpResquest& request, int epollFd);

#endif
