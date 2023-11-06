/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:16:40 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/06 21:58:09 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>

int main() {
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
				readData(conn);
			if (conn->state == HANDLING)
				handleRequest(epollFd, conn);
			if (conn->state == CGI)
				readCGI(conn);
			if (conn->state == WRITING && events[i].events & EPOLLOUT)
				writeData(conn);
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
				handleError(conn);
			if (conn->state == CLOSING)
				closeConnection(epollFd, conn);
        }
    }
    return 0;
}
