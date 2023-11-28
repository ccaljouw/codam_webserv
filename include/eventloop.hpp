/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   eventloop.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 18:13:17 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/28 16:36:04 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

# include <iostream>
# include <string>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <ctime>

# include "Server.hpp"

enum	states {
	LISTENING,
	READING,
	HANDLING,
	IN_CGI,
	WRITING,
	CLOSING
};

struct connection
{
	int							fd;
	int							cgiFd;
	pid_t						cgiPID; 
	states						state;
	std::string					request;
	std::string					response;
	Server						*server;
	std::time_t					time_last_request;
	int							nr_of_requests;
	bool						close_after_response;
};

// handlers.cpp
void	newConnection(int epollFd, int serverFd, Server *server);
void	readData(connection *conn);
void	handleRequest(int epollFd, connection *conn);
void	readCGI(int epollFd, connection *conn);
void	writeData(connection *conn);
void	closeConnection(int epollFd, connection *conn);

// registerEvents.cpp
int		register_server(int epollFd, int fd, Server *server);
int		register_client(int epollFd, int severFd, Server *Server);
int		register_CGI(int epollFd, int cgiFd, connection *conn);

#endif