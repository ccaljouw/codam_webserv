/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   registerEvents.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:48:35 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/10 11:21:30 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>

// TODO: check connection 
int	register_server(int epollFd, int fd, Server *server)
{
	struct epoll_event 	event;
	connection			*conn;
	
	conn = new connection{fd, 0, LISTENING, "", "", server};
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
	return (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event));
}

// TODO: check connection
int	register_client(int epollFd, int fd, Server *server)
{
	struct epoll_event 	event;
	connection			*conn;

	conn = new connection{fd, 0, READING,  "", "", server}; //add server};
	event.events = EPOLLIN | EPOLLOUT;   
    event.data.ptr = conn;
	return (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event));
}

int	register_CGI(int epollFd, int cgiFd, connection *conn)
{
	struct epoll_event 	event;
	
	conn->cgiFd = cgiFd;
	event.events = EPOLLIN;   
    event.data.ptr = conn;
	return (epoll_ctl(epollFd, EPOLL_CTL_ADD, cgiFd, &event));
}
