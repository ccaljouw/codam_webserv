/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:16:40 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/11 22:51:03 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "eventloop.hpp"
#include "Config.hpp"
 #include <string.h>

int main(int argc, char **argv) {

	int 				epollFd;
	struct epoll_event	events[MAX_EVENTS];
	std::list<Server>	servers;
	try {
		Config conf;
		if (argc == 2) // deze check verplaatsen naar binnen Config class?
			conf.setFile(argv[1]);
		else if (argc > 2) // wiilen we dit of negeren we gewoon de rest als er meer is meegegeven?
			throw std::runtime_error("invallid nr of arguments");
		if ((epollFd = epoll_create(1)) == -1)
			throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
		if ((servers = initServers(conf.getServers(), epollFd)).size() == 0)
			throw std::runtime_error("no succesfull server configuration, terminating programm");
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << "\033[31;1mError\n" << e.what() << "\033[0m" << std::endl;
		return 1;
	}
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
	// delete remaining conn structs by setting flag in signal handling?
    return 0;
}
