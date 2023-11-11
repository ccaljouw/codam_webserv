/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:16:40 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/11 10:43:12 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "eventloop.hpp"
#include "Config.hpp"

int main(int argc, char **argv) {

	Config conf(argc, argv);
	if (conf.getError() == true)
		return 1;

	// Create epoll file descriptor
    int 				epollFd = epoll_create(1);
    struct epoll_event	events[MAX_EVENTS];
	std::list<Server> servers = initServers(conf.getServers(), epollFd);
	
	// loop for events
    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 0);
		
        for (int i = 0; i < numEvents; i++) {
			connection *conn = static_cast<connection *>(events[i].data.ptr);
            if (conn->state == LISTENING && events[i].events & EPOLLIN)
				newConnection(epollFd, conn->fd, conn->server);
			if (conn->state == CLOSING || events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
				closeConnection(epollFd, conn);
			if ((conn->state == READING) && events[i].events & EPOLLIN)
				readData(conn);
			if (conn->state == HANDLING)
				handleRequest(epollFd, conn);
			if (conn->state == IN_CGI && events[i].events & EPOLLIN)
				readCGI(epollFd, conn);
			if (conn->state == WRITING && events[i].events & EPOLLOUT)
				writeData(conn);
        }
    }
	// clear serverlist in signal handler!!
    return 0;
}
