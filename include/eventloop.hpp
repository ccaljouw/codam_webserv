/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   eventloop.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 18:13:17 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/09 12:19:13 by bfranco       ########   odam.nl         */
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

# include <Server.hpp>

# define MAX_EVENTS 10
# define BUFFER_SIZE 10240

enum	states {
	LISTENING,
	CONNECTED,
	READING,
	HANDLING,
	IN_CGI,
	WRITING,
	CLOSING
};

struct connection
{
	int			fd;
	int			cgiFd;
	states		state;
	std::string	request;
	std::string	response;
};

// handlers.cpp
void	newConnection(int epollFd, int fd);
void	readData(int epollFd, connection *conn);
void	handleRequest(int epollFd, connection *conn);
void	readCGI(int epollFd, connection *conn);
void	handleError(connection *conn);
void	writeData(int epollFd, connection *conn);
void	closeConnection(int epollFd, connection *conn);

// registerEvents.cpp
void	register_server(int epollFd, int fd);
void	register_client(int epollFd, int severFd);
void	register_CGI(int epollFd, int cgiFd, connection *conn);
void	modifyEvent(int epollFd, int flag, connection *conn);

#endif