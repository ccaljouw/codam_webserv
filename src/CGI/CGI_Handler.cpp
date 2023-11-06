/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 17:10:37 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"

CGI::CGI(connection *conn, int epollFd) : _epollFd(epollFd), _status(0)
{

	int	fd[2];
	try
	{
		if (pipe(fd) == -1)
			throw std::runtime_error("pipe failed");
		_fdIn = fd[0];
		_fdOut = fd[1];
		register_CGI(conn, _epollFd, _fdIn);
	}
	catch(const std::runtime_error& e)
	{
		_status = 1;
		std::cerr << e.what() << '\n';
	}
}

void	CGI::closeFds() const
{
	close(_fdIn);
	close(_fdOut);
}

CGI::~CGI() {};
int CGI::getStatus() const	{ return (_status); }
int CGI::getFdIn() const		{ return (_fdIn); }
int CGI::getFdOut() const		{ return (_fdOut); }

void	execChild(const Uri& uri, CGI &cgi)
{
	char	*program = const_cast<char *>(uri.getPath().c_str());
	char	*argv[] = {const_cast<char *>("/bin/ls"), const_cast<char *>("-la"), NULL};
	char	**env = NULL;
	
	(void)program;
	std::cout << "exec child" << std::endl;
	// if (dup2(cgi.getFdOut(), 1) == -1)
	// {
	// 	write(cgi.getFdOut(), "status: 500\r\n\r\n", 15);
	// 	cgi.closeFds();
	// 	return ;
	// }
	cgi.closeFds();
	// execve(program, argv, env);
	execve(argv[0], argv, env);

}

int cgiHandler(const Uri& uri, connection *conn, int epollFd)
{
	CGI	cgi(conn, epollFd);

	if (cgi.getStatus() == 1)
		return 1;
	
	int pid = fork();
	if (pid == -1)
		return 1;
	else if (pid == 0)
		execChild(uri, cgi);
	else
		close(cgi.getFdIn());
	return 0;
}
