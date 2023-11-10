/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handlers.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/10 09:58:18 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

// TODO: check errors, check duplicates
// TODO: check what to do if backlog is full?
void	newConnection(int epollFd, int serverFd) 
{
	int					fd;
	
	std::cout << "new connection request" << std::endl;
	fd = accept(serverFd, nullptr, nullptr);
	if (fd == -1)
		return ;
	if (register_client(epollFd,  fd) == -1)
	{
		//send internal server error cannot use write via epoll??
		close(fd);
	}
}

void readData(connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) // what recv flag to use??
	{
		std::cout << "\n\033[0;33m" << buffer << "\033[0m\n" << std::endl; //for testing
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
	}
	switch(bytesRead)
	{
		case -1:
			std::cout << "error reading" << std::endl; // for testing
			setErrorResponse(conn, 500);
			break;
		case 0:
			std::cout << "nothing to read" << std::endl; //for testing
			checkTimeout(conn);
			break;
		case BUFFER_SIZE:
			conn->state = READING;
			break;
		default:
			conn->state = HANDLING;
	}
}

// TODO: switch tree with utils helpers
void handleRequest(int epollFd, connection *conn) 
{
	(void)epollFd;

	try {
		// Process the request data
		HttpRequest request(conn->request);
	
		// Handle parsing error
		if (request.getRequestStatus() != 200) {
			setErrorResponse(conn, request.getRequestStatus());
		} 
		
		// handle CGI
		else if (request.uri.getExecutable() == "cgi-bin") { //todo:make configurable
			//case error in cgi handler
			if (cgiHandler(request.getUri(), conn, epollFd, request.getEnvArray()) == 1 ) 
				setErrorResponse(conn, 500);	
			else
				// conn->state = CLOSING;
				conn->state = IN_CGI;
		}
		
		//handle GET
		else if (request.getMethod() == "GET") {
			if (request.uri.isValidExtension() ) {
				HttpResponse response(request);
				response.setBody("." + request.uri.getPath());
				setResponse(conn, response);
			}
			else
				throw HttpRequest::parsingException(501, "Extension not supported"); 
		}
		
		//handle POST		
		else if (request.getMethod() == "POST") {
			if (request.uri.isValidExtension() ) {
				std::cout << "add code for cgi POST" << std::endl;
				throw HttpRequest::parsingException(405, "POST METHOD not supported yet"); // todo: remove line

			}
			else
				throw HttpRequest::parsingException(501, "Extension not supported"); 
		}

		// handle DELETE
		else if (request.getMethod() == "DELETE") {
			if (request.uri.isValidExtension() ) {
				std::cout << "add code for cgi DELete" << std::endl;
				throw HttpRequest::parsingException(405, "DELETE METHOD not supported yet"); // todo: remove line
			}
			else
				throw HttpRequest::parsingException(501, "Extension not supported");
		}

		// handle unsupported methods
		else
			throw HttpRequest::parsingException(405, "METHOD not supported");

		
	} catch (const HttpRequest::parsingException& exception) {
		std::cout << "Error: " << exception.what() << std::endl;
		setErrorResponse(conn, exception.getErrorCode());
		}
}

void readCGI(int epollFd, connection *conn)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	std::cout << "read data CGI" << std::endl;
    if ((bytesRead = read(conn->cgiFd, buffer, sizeof(buffer))) > 0) {
		conn->response.append(buffer, static_cast<long unsigned int>(bytesRead));
    }
	if (bytesRead < BUFFER_SIZE)
	{
		std::cout << "\033[34;1m" << buffer << "\033[0m\n" << std::endl;
		close(conn->cgiFd);
		epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->cgiFd, nullptr);
		conn->state = WRITING;
	}
	if (conn->response.empty() || bytesRead == -1)
	{
		setErrorResponse(conn, 500);
		std::cout << "error reading CGI" << std::endl;
	}
}


// TODO: check errors
void writeData(connection *conn) 
{
	int			len;
	
	conn->state = WRITING;
	std::cout << "\033[32;1m" << conn->response << "\033[0m\n" << std::endl;
	len = std::min(static_cast<int>(conn->response.length()), BUFFER_SIZE);
    len = send(conn->fd, conn->response.c_str(), conn->response.length(), 0);
	if (len == -1)
	{
		std::cout << "error sending" << std::endl; // for testing
		conn->state = CLOSING;
	}
	else if (len < static_cast<int>(conn->response.length()))
	{
		std::cout << "not finished writing yet" << std::endl; // for testing
		conn->response = conn->response.substr(len, conn->response.npos);
		conn->state = WRITING;
	}
	else
	{
		std::cout << "Response sent" << std::endl; //for testing
		conn->response.clear();
		conn->state = READING;
	}
}

// TODO: handle errors??
void	closeConnection(int epollFd, connection *conn)
{
	std::cout << "close connection" << std::endl;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
    close(conn->fd);
	delete conn;
    std::cout << "Connection closed" << std::endl;
}

