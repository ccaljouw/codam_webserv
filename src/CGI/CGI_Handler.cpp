/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI_Handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 12:51:38 by bfranco           #+#    #+#             */
/*   Updated: 2023/11/06 15:07:26 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"
#include "eventloop.hpp"

CGI::CGI(int epollFd) : _epollFd(epollFd), _status(0)
{

	int	fd[2];
	try
	{
		pipe(fd);
		// if (pipe(fd) == -1)
		// 	throw std::runtime_error("pipe failed");
		_fdIn = fd[0];
		_fdOut = fd[1];
		register_CGI(_epollFd, _fdIn);
	}
	catch(const std::runtime_error& e)
	{
		_status = 1;
		std::cerr << e.what() << '\n';
	}
}

CGI::~CGI() {};

int CGI::getStatus() const	{ return (_status); }
int CGI::getFdIn() const		{ return (_fdIn); }
int CGI::getFdOut() const		{ return (_fdOut); }

void	execChild(const Uri& uri, HttpResponse& response, CGI &cgi)
{
	
	std::cout << "in execChild" << std::endl;
	char	*program = const_cast<char *>(uri.getPath().c_str());
	char	*argv[] = {const_cast<char *>("/bin/echo"), const_cast<char *>("hello")};
	char		**env = NULL;
	
	if (dup2(cgi.getFdOut(), 1) == -1)
	{
		std::cout << "dup error" << std::endl;
		response.setStatusCode(500);
		return ;
	}
	close(cgi.getFdIn());
	// close(cgi.getFdOut());
	std::cout << "before execve" << std::endl;
	execve(program, argv, env);
	
}

void cgiHandler(const Uri& uri, HttpResponse& response, int epollFd)
{
	CGI	cgi(epollFd);

	std::cout << "in cgi" << std::endl;
	if (cgi.getStatus() == 1)
	{
		response.setStatusCode(500);
		return ;
	}
	
	int pid = fork();
	if (pid == -1)
	{
		std::cout << "PID == -1" << std::endl;
		response.setStatusCode(500);
		return;
	}
	else if (pid == 0)
		execChild(uri, response, cgi);
	// else
	// 	execParent(uri, response, cgi);
}
