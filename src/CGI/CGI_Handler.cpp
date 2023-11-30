/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/30 11:37:00 by cwesseli      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

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


	std::cout << "cgi fd = " << cgi.getFdIn() << std::endl;
	if (dup2(cgi.getFdOut(), STDOUT_FILENO) == -1 || dup2(oldFd, STDIN_FILENO) == -1)
	{
		cgi.closeFds();
		std::exit(1);
	}
	// close(cgi.getFdIn());
	cgi.closeFds();
	close(oldFd);
	execve(argv[0], argv, env);
	std::exit(1);
}

int	writeBody(const HttpRequest& req, int *fd)
{
	
	std::string	body = req.getBody() + "\0";
	while (1)
	{
		if (write(fd[1], body.c_str(), BUFFER_SIZE) == -1)
		{
			std::cerr << strerror(errno) << std::endl;
			close(fd[0]);
			close(fd[1]);
			return (-1);
		}
		if (body.length() > BUFFER_SIZE)
			body = body.substr(BUFFER_SIZE, body.npos);
		else
			break;
	}
	close(fd[1]);
	return (fd[0]);
}

int cgiHandler(const HttpRequest& req, connection *conn, int epollFd)
{
	int			fd[2];

	if (pipe(fd) == -1)
		return (-1);
	
	CGI	cgi(epollFd, conn);
	if (cgi.getStatus() == 1)
	{
		cgi.closeFds();
		return (1);
	}

	int	pid = fork();
	if (pid == -1)
	{
		cgi.closeFds();
		return (1);
	}
	else if (pid == 0)
		execChild(req, cgi, fd[0]);
	else
	{
		conn->cgiPID = pid;
		close(cgi.getFdOut());
		if (writeBody(req, fd) == -1)
		{
			close(fd[0]);
			return (1);
		}
		close(fd[0]);
	}
	return (0);
}
