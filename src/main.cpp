/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:16:40 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/09 16:42:32 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "eventloop.hpp"
#include "Config.hpp"

int main(int argc, char **argv) {

	if (argc == 1) {
		Config conf;
		std::cout << conf.getServers().front()._serverName << std::endl;}
	else if (argc == 2)
		Config conf(argv[1]);
	else
		return (1);


	// Create epoll file descriptor
    int 				epollFd = epoll_create(1);
    struct epoll_event	events[MAX_EVENTS];
    
	// Create and init socket for the server
	Server server(8080, epollFd);
	
	// loop for events
    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 0);
		
        for (int i = 0; i < numEvents; i++) {
			connection *conn = static_cast<connection *>(events[i].data.ptr);
            if (conn->state == LISTENING && events[i].events & EPOLLIN)
				newConnection(epollFd, conn->fd);
			if ((conn->state == CONNECTED || conn->state == READING) \
								&& events[i].events & EPOLLIN)
				readData(epollFd, conn);
			if (conn->state == HANDLING)
				handleRequest(epollFd, conn);
			if (conn->state == IN_CGI && events[i].events & EPOLLIN)
				readCGI(epollFd, conn);
			if (conn->state == WRITING && events[i].events & EPOLLOUT)
				writeData(epollFd, conn);
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
				handleError(conn);
			if (conn->state == CLOSING)
				closeConnection(epollFd, conn);
        }
    }
    return 0;
}
