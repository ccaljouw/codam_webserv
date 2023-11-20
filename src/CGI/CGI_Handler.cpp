/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/17 16:09:12 by bfranco       ########   odam.nl         */
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

void	execChild(const HttpRequest& req, CGI &cgi)
{
	char	program[req.uri.getPath().size() + 2];
	char	*argv[] = {program, NULL};
	char	**env = req.getEnvArray();
	getProgramPath(req.uri, program);
	
	std::cerr << "program = " << argv[0] << std::endl;
	// if (dup2(cgi.getFdOut(), STDOUT_FILENO) == -1)
	// {
	// 	write(cgi.getFdOut(), "status: 500\r\n\r\n", 15);
	// 	cgi.closeFds();
	// 	return ;
	// }
	// char	*env2[] = {
	// 	strdup("AUTH_TYPE="),
	// 	strdup("CONTENT_LENGTH=1024"),
	// 	strdup("CONTENT_TYPE=text/html"),
	// 	strdup("GATEWAY_INTERFACE="),
	// 	strdup("HTTP_ACCEPT="),
	// 	strdup("HTTP_ACCEPT_CHARSET="),
	// 	strdup("HTTP_ACCEPT_ENCODING="),
	// 	strdup("HTTP_ACCEPT_LANGUAGE="),
	// 	strdup("HTTP_FORWARDED="),
	// 	strdup("HTTP_METHOD=POST"),
	// 	strdup("HTTP_HOST=http://localhost:8080"),
	// 	strdup("HTTP_PROXY_AUTHORIZATION="),
	// 	strdup("HTTP_USER_AGENT="),
	// 	strdup("PATH_INFO="),
	// 	strdup("PATH_TRANSLATED="),
	// 	strdup("QUERY_STRING=name=banana"),
	// 	strdup("REMOTE_ADDR="),
	// 	strdup("REMOTE_HOST="),
	// 	strdup("REMOTE_USER="),
	// 	strdup("REQUEST_METHOD="),
	// 	strdup("SCRIPT_NAME=test.py"),
	// 	strdup("SERVER_NAME="),
	// 	strdup("SERVER_PORT="),
	// 	strdup("SERVER_PROTOCOL="),
	// 	strdup("SERVER_SOFTWARE="),
	// 	strdup("HTTP_COOKIE=id=hello value=3"),
	// 	NULL
	// };

	for (int i = 0; env[i]; i++)
		std::cout << "env[" << i << "] = " << env[i] << std::endl;
	dup2(cgi.getFdOut(), STDOUT_FILENO);
	dup2(cgi.getFdIn(), STDIN_FILENO);
	std::string body = req.getBody();
	if (body.size() > 0)
		write(STDIN_FILENO, (body+"\0").c_str(), body.size()+1);
	// close(cgi.getFdIn());
	cgi.closeFds();
	std::cerr << "before execve" << std::endl;
	execve(argv[0], argv, env);
}

int cgiHandler(const HttpRequest& req, connection *conn, int epollFd)
{
	CGI	cgi(epollFd, conn);
	
	std::cerr << "in CGI" << std::endl;
	if (cgi.getStatus() == 1)
	{
		cgi.closeFds();
		return 1;
	}
	int pid = fork();
	if (pid == -1)
	{
		cgi.closeFds();
		return 1;
	}
	else if (pid == 0)
		execChild(req, cgi);
	else
		close(cgi.getFdOut());
	return 0;
}
