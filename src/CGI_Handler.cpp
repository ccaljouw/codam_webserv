/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 14:22:59 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"

CGI::CGI(int epollFd) : _epollFd(epollFd), _status(0)
{
	int	fd[2];

	try
	{
		if (pipe(fd) == -1)
			throw std::runtime_error("pipe failed");
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

const int CGI::getStatus() const	{ return (_status); }
const int CGI::getFdIn() const		{ return (_fdIn); }
const int CGI::getFdOut() const		{ return (_fdOut); }

void	execChild(const Uri& uri, HttpResponse& response, CGI cgi)
{
	
	char	*program = static_cast<char *>uri.getPath();
	char	*argv[] = {program, NULL};
	char	**env = NULL;
	
	if (std::dup2(cgi.getFdOut(), 1) == -1)
	{
		response.setStatus(500);
		return ;
	}
	close(cgi.getFdIn());
	std::execve(program, argv, env);
	
}

void cgiHandler(const Uri& uri, HttpResponse& response, int epollFd)
{
	CGI	cgi(epollFd);

	if (cgi.getStatus() == 1)
	{
		response.setStatus(500);
		return ;
	}
	
	int pid = fork();
	if (pid == -1)
	{
		response.setStatus(500);
		return;
	}
	else if (pid == 0)
		execChild(uri, response, cgi);
	else
		execParent(uri, response, cgi);
}
