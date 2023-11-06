/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 23:45:10 by cariencaljo       #+#    #+#             */
/*   Updated: 2023/11/06 13:26:47 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>
#include <webServ.hpp>
#include <unistd.h>

// TODO: check errors, check duplicates
// TODO: check what to do if backlog is full?
void	newConnection(int epollFd, int fd) 
{
	std::cout << "new connection request" << std::endl;
	register_client(epollFd,  fd);
}

// TODO: check errors
void readData(connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	std::cout << "read data" << std::endl;
	conn->state = READING;
    if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) {
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
		std::cout << "bytes read: " << bytesRead << std::endl;
		if (bytesRead < BUFFER_SIZE)
			conn->state = HANDLING;
    }
   	else if (conn->request.empty()) {
		std::cout << "nothing to read" << std::endl;
		conn->state = CLOSING;
    }
}

// TODO: a lot :)
void handleRequest(int epollFd, connection *conn) 
{
    // Process the request data
	std::cout << "handle request" << std::endl;
	HttpRequest request(conn->request);
	HttpResponse response(request);
	// if (to CGI)
	(void)epollFd;
		// call CGI handler
		// conn->state = GGI
	// else
	{
		conn->response = response.serializeResponse();
		conn->request.clear();
		std::cout << "Response ready" << std::endl;
		conn->state = WRITING; // change to response ready status?
	}
}

void readCGI(connection *conn)
{
	(void)conn;
	// read CGI pipe and serializeRespons();
	// conn->response = response.serializeResponse();
	// conn->request.clear();
	// std::cout << "Response ready" << std::endl;
	// conn->state = WRITING; 
}

// TODO: should only write buffer size?
// TODO: check errors
// TODO: set status CLOSING when response whas error
void writeData(connection *conn) 
{
    // Send the response
	conn->state = WRITING;
	std::cout << "write data" << std::endl;
    send(conn->fd, conn->response.c_str(), conn->response.length(), 0);
	conn->response.clear();
    std::cout << "Response sent" << std::endl;
	conn->state = CONNECTED;
}

// TODO: handle errors
void	closeConnection(int epollFd, connection *conn)
{
	std::cout << "close connection" << std::endl;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
    close(conn->fd);
	delete conn;
    std::cout << "Connection closed" << std::endl;
}

void	handleError(connection *conn)
{
	conn->state = CLOSING;
}