/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handlers.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/06 09:46:45 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>
# include <unistd.h>

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
        conn->client->append_requestData(buffer, static_cast<long unsigned int>(bytesRead));
		std::cout << "bytes read: " << bytesRead << std::endl;
		if (bytesRead < BUFFER_SIZE)
			conn->state = HANDLING;
    }
   	else if (conn->client->get_request().empty()) {
		std::cout << "nothing to read" << std::endl;
		conn->state = CLOSING;
    }
}

// TODO: a lot :)
void handleRequest(connection *conn) 
{
    // Process the request data
	std::cout << "handle request" << std::endl;
	std::string request = conn->client->get_request();
	int	length =  static_cast<int>(request.length()) + 22;
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " \
			+ std::to_string(length) + "\n\nmessage received:\n" + request + "\n\n";
	conn->client->set_response(response);
	conn->client->clear_request();
	std::cout << "Response ready" << std::endl;
	conn->state = WRITING; // change to response ready status?
}

// TODO: should only write buffer size?
// TODO: check errors
// TODO: set status CLOSING when response whas error
void writeData(connection *conn) 
{
    // Send the response
	conn->state = WRITING;
	std::cout << "write data" << std::endl;
	std::string response = conn->client->get_response();
    send(conn->fd, response.c_str(), response.length(), 0);
	conn->client->clear_response();
    std::cout << "Response sent" << std::endl;
	conn->state = CONNECTED;
}

// TODO: handle errors
void	closeConnection(int epollFd, connection *conn)
{
	std::cout << "close connection" << std::endl;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, NULL);
    close(conn->fd);
	delete conn->client;
	delete conn;
    std::cout << "Connection closed" << std::endl;
}

void	handleError(connection *conn)
{
	conn->state = CLOSING;
}