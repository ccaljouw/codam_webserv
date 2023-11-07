/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/07 16:59:39 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"
#include <string>

CGI::CGI(int epollFd, connection *conn) : _epollFd(epollFd), _status(0), _fdIn(-1), _fdOut(-1)
{
	int	fd[2];
	try
	{
		if (pipe(fd) == -1)
			throw std::runtime_error("pipe failed");
		else
		{
			_fdIn = fd[0];
			_fdOut = fd[1];
			register_CGI(_epollFd, _fdIn, conn);
		}
	}
	catch(const std::runtime_error& e)
	{
		_status = 1;
		std::cerr << e.what() << '\n';
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
int CGI::getStatus() const	{ return (_status); }
int CGI::getFdIn() const		{ return (_fdIn); }
int CGI::getFdOut() const		{ return (_fdOut); }

char	*getProgramPath(const Uri& uri, char *program)
{
	std::string	path = uri.getPath();
	std::cout << "path = " << path << std::endl;
	program[0] = '.';
	for (int i = 0; i < static_cast<int>(path.size()); i++)
		program[i + 1] = path[i];
	program[path.size() + 1] = '\0';
	return (program);
}

void	execChild(const Uri& uri, CGI &cgi, char **env)
{
	char	program[uri.getPath().size() + 2];
	char	*argv[] = {program, NULL};
	// char	**env = uri.getHeadersArray();
	getProgramPath(uri, program);
	
	std::cout << "program = " << argv[0] << std::endl;
	// if (dup2(cgi.getFdOut(), STDOUT_FILENO) == -1)
	// {
	// 	write(cgi.getFdOut(), "status: 500\r\n\r\n", 15);
	// 	cgi.closeFds();
	// 	return ;
	// }
	dup2(cgi.getFdOut(), STDOUT_FILENO);
	// cgi.closeFds();
	close(cgi.getFdIn());
	execve(argv[0], argv, env);
}

int cgiHandler(const Uri& uri, connection *conn, int epollFd, char **env)
{
	CGI	cgi(epollFd, conn);

	if (cgi.getStatus() == 1)
	{
		cgi.closeFds();
		return 1;
	}
	
	int pid = fork();
	if (pid == -1) //close FdIn and FdOut?
		return 1;
	else if (pid == 0)
		execChild(uri, cgi, env);
	else
		close(cgi.getFdOut());
	return 0;
}
