/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handlers.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/07 09:47:17 by carlo         ########   odam.nl         */
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
	(void)epollFd;

	try {
    // Process the request data
	std::cout << "handle request" << std::endl;
	HttpRequest request(conn->request);
	
	//case parsing error:
	if (request.getRequestStatus() != 200) {
		conn->response = HttpResponse(request).serializeResponse();
		conn->request.clear();
		std::cout << "Response ready" << std::endl;
		conn->state = WRITING; // change to response ready status?		
	
	// case cgi
	// } else {
	// 	if (request.uri.getExecutable() == "cgi-bin") { //todo:make configurable
	//	call CGI handler with pathinfo and array of env variable
	//	std::string pathinfo = request.uri.getPathInfo();
	//	char **headerArray = request.getHeadersArray(); // todo:: needs to be freed!!
	
		
		
	//	case error in cgi handler
		// if (cgi_handler(conn->request) == 1 ) {
		// 	HttpResponse respons(request);
		// 	respons.setStatusCode(500);
		// 	conn->response = respons.serializeResponse();
		// 	conn->request.clear();
		// 	std::cout << "Response ready" << std::endl;
		// 	conn->state = WRITING; // change to response ready status?		
		// } else {
		// cgi handles it from here?
		// conn->state = GGI	
		// }
		
	//case handel self		
	} else if (request.getMethod() == "GET") {
			HttpResponse response(request);
			response.setBody("." + request.uri.getPath()); //todo ugly solution maybe better parsing?
			conn->response = response.serializeResponse();
			conn->request.clear();
			std::cout << "Response ready" << std::endl;
			conn->state = WRITING; // change to response ready status?
	}
	// todo move to error handeling 
	} catch (const HttpRequest::parsingException& exception) {
		std::cout << "Error: " << exception.what() << std::endl;
		HttpResponse response;
		response.setStatusCode(exception.getErrorCode());
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
// TODO: set status CLOSING when response was error
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