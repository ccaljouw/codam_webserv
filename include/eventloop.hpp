/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   eventloop.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 18:13:17 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/11 22:30:17 by cariencaljo   ########   odam.nl         */
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

# include "Server.hpp"

# define MAX_EVENTS 10
# define BUFFER_SIZE 1024

enum	states {
	LISTENING,
	READING,
	HANDLING,
	IN_CGI,
	WRITING,
	CLOSING
};

//todo map > unordered map for faster lookup
struct connection
{
	int							fd;
	int							cgiFd;
	states						state;
	std::string					request;
	std::string					response;
	Server		*server;
	// int			requests; 
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