/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   registerEvents.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:48:35 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/14 09:13:08 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <string.h>

int	register_server(int epollFd, int fd, Server *server)
{
	struct epoll_event 	event;
	connection			*conn;
	
	conn = new connection{fd, 0, LISTENING, std::string(""), std::string(""), server, std::time(nullptr), 0};
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		close (fd);
		delete conn;
		return 1;
	}
	return 0;
}

int	register_client(int epollFd, int fd, Server *server)
{
	struct epoll_event 	event;
	connection			*conn;

	conn = new connection{fd, 0, READING,  std::string(""), std::string(""), server, std::time(nullptr), 0};
	event.events = EPOLLIN | EPOLLOUT;
    event.data.ptr = conn;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		// try again and if not successfull after 3 times send error and close
		close(fd);
		delete conn;
		return 1;
	}
	return 0;
}

int	register_CGI(int epollFd, int cgiFd, connection *conn)
{
	struct epoll_event 	event;
	
	conn->cgiFd = cgiFd;
	event.events = EPOLLIN;   
    event.data.ptr = conn;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, cgiFd, &event) == -1)
	{
		setErrorResponse(conn, 500);
		return 1;
	}
	return 0;
}
