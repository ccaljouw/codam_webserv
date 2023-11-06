/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   registerEvents.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:48:35 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/06 17:32:07 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>

// TODO: check connection 
void	register_server(int epollFd, int fd)
{
	struct epoll_event 	event;
	connection			*conn;
	
	conn = new connection{fd, 0, LISTENING, "", ""};
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = conn;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
}

// TODO: check connection
// EPOLLET could make the loop more efficient but only works if reading 
// and writing is done in one block, cannot be used while reading/writing 
// only buffersize  
void	register_client(int epollFd, int serverFd)
{
	struct epoll_event 	event;
	connection			*conn;
	int					fd;
	
	fd = accept(serverFd, nullptr, nullptr);
	conn = new connection{fd, 0, CONNECTED,  "", ""};
	event.events = EPOLLIN | EPOLLOUT;   
    event.data.ptr = conn;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
}

void	register_CGI(int epollFd, int cgiFd, connection *conn)
{
	struct epoll_event 	event;
	
	conn->cgiFd = cgiFd;
	event.events = EPOLLIN;   
    event.data.ptr = conn;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, cgiFd, &event);
}
