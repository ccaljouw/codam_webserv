/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:16:40 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/26 20:12:02 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "eventloop.hpp"
#include "Config.hpp"
#include "webServ.hpp"
#include <string.h>
#include <signal.h>

int	g_shutdown_flag = 0;

int main(int argc, char **argv) {

	int 				epollFd;
	struct epoll_event	events[MAX_EVENTS];
	std::list<Server>	servers;

	signal(SIGINT, handleSignal);
	try {
		Config conf(argc, argv);
		if (conf.getError() == true)
			throw std::runtime_error("invallid arguments");
		std::exit(0);
		if ((epollFd = epoll_create(1)) == -1)
			throw std::runtime_error("epoll_create: " + std::string(strerror(errno)));
		// if ((servers = initServers(conf.getServers(), epollFd)).size() == 0)
		// 	throw std::runtime_error("no succesfull server configuration, terminating programm");
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
	while (!g_shutdown_flag) {
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 0);
		
		for (int i = 0; i < numEvents; i++) {
			connection *conn = static_cast<connection *>(events[i].data.ptr);
			if (events[i].events & EPOLLIN && conn->state == LISTENING)
				newConnection(epollFd, conn->fd, conn->server);
			if (events[i].events & EPOLLIN && conn->state == READING)
				readData(conn);
			if (conn->state == HANDLING)
				handleRequest(epollFd, conn);
			if (events[i].events & EPOLLIN && conn->state == IN_CGI)
				readCGI(epollFd, conn);
			if (events[i].events & EPOLLOUT && conn->state == WRITING)
				writeData(conn);
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || conn->state == CLOSING)
				closeConnection(epollFd, conn);
		}
	}
	std::cout << CYAN << "clean up" << RESET << std::endl;
	int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 0);
	for (int i = 0; i < numEvents; i++) {
		connection *conn = static_cast<connection *>(events[i].data.ptr);
		if (conn->state != LISTENING)
			closeConnection(epollFd, conn);
	}
	close (epollFd);
    return 0;
}
