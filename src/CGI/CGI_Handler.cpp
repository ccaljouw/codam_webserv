/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/20 16:09:46 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"
#include <string.h>

CGI::CGI(int epollFd, connection *conn) : _epollFd(epollFd), _status(0), _fdIn(-1), _fdOut(-1)
{
	int	fd[2];
	try
	{
		if (pipe(fd) == -1)
			throw std::runtime_error("CGI pipe: " + std::string(strerror(errno)));
		else
		{
			_fdIn = fd[0];
			_fdOut = fd[1];
			if (register_CGI(_epollFd, _fdIn, conn))
				throw std::runtime_error("registerCGI: " + std::string(strerror(errno)));
		}
	}
	catch(const std::runtime_error& e)
	{
		_status = 1;
		std::cerr << "\033[31;1mError\n" << e.what() << "\n\033[0m";
	}
}

void	CGI::closeFds()
{
	if (_fdIn != -1)
	{
		close(_fdIn);
		_fdIn = -1;
	}
	if (_fdOut != -1)
	{
		close(_fdOut);
		_fdOut = -1;
	}
}

CGI::~CGI() {};
int		CGI::getStatus() const		{ return (_status); }
int		CGI::getFdIn() const		{ return (_fdIn); }
int		CGI::getFdOut() const		{ return (_fdOut); }

char	*getProgramPath(const Uri& uri, char *program)
{
	std::string	path = uri.getPath();
	program[0] = '.';
	for (int i = 0; i < static_cast<int>(path.size()); i++)
		program[i + 1] = path[i];
	program[path.size() + 1] = '\0';
	return (program);
}

void	execChild(const HttpRequest& req, CGI &cgi, int oldFd)
{
	char	program[req.uri.getPath().size() + 2];
	char	*argv[] = {program, NULL};
	char	**env = req.getEnvArray();
	getProgramPath(req.uri, program);

	dup2(cgi.getFdOut(), STDOUT_FILENO);
	dup2(oldFd, STDIN_FILENO);
	// close(cgi.getFdIn());
	// cgi.closeFds();
	execve(argv[0], argv, env);
}

int	writeBody(const HttpRequest& req)
{
	int			fd[2];
	std::string	body = req.getBody() + "\0";

	if (pipe(fd) == -1)
		return (-1);
	if (write(fd[1], body.c_str(), body.size()) == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	close(fd[1]);
	return (fd[0]);
}

int cgiHandler(const HttpRequest& req, connection *conn, int epollFd)
{
	int	oldFd = writeBody(req);

	if (oldFd == -1)
		return (1);

	CGI	cgi(epollFd, conn);
	if (cgi.getStatus() == 1)
	{
		cgi.closeFds();
		return 1;
	}

	int	pid = fork();
	if (pid == -1)
	{
		cgi.closeFds();
		return 1;
	}
	else if (pid == 0)
		execChild(req, cgi, oldFd);
	else
	{
		close(cgi.getFdOut());
	}
	return 0;
}
