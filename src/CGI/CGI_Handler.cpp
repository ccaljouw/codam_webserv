/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI_Handler.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/06 12:51:38 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/06 20:11:48 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI_Handler.hpp"
#include "HttpRequest.hpp"

CGI::CGI(int epollFd, connection *conn) : _epollFd(epollFd), _status(0)
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
	// if (dup2(cgi.getFdOut(), STDOUT_FILENO) == -1)
	// {
	// 	write(cgi.getFdOut(), "status: 500\r\n\r\n", 15);
	// 	cgi.closeFds();
	// 	return ;
	// }
	close(cgi.getFdIn());
	dup2(cgi.getFdOut(), STDOUT_FILENO);
	// execve(program, argv, env);
	execve(argv[0], argv, env);

}

int cgiHandler(const Uri& uri, connection *conn, int epollFd)
{
	CGI	cgi(epollFd, conn);

	if (cgi.getStatus() == 1)
		return 1;
	
	int pid = fork();
	if (pid == -1) //close FdIn and FdOut?
		return 1;
	else if (pid == 0)
		execChild(uri, cgi);
	else
		close(cgi.getFdOut());
	return 0;
}
