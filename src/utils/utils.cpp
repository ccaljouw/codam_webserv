/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/22 21:57:55 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/04 14:20:48 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

#include <string>
#include <fstream>
#include <signal.h>


std::string getTimeStamp() {
	//get current time
    std::time_t now = std::time(nullptr);
	
	//store current time in struct as gmt time
	std::tm* timeInfoGmt = std::gmtime(&now);
    
	// Define the format for the output string
    char buffer[40];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfoGmt);
    return buffer;
}
	

void	setErrorResponse(connection *conn, int error)
{
	HttpResponse response;
	std::string errorHtmlPath = generateErrorPage(error);
	response.setStatusCode(error);
	if (error == 408 || error == 429 || error == 500 || error == 504) {
		conn->close_after_response = 1;
		response.headers->setHeader("Connection", "close");		
	}
	
	std::ifstream f(errorHtmlPath);
	if (f.good())
		response.reSetBody(errorHtmlPath, false);
	
	//todo : remove tmp file or not
	// if (std::remove(errorHtmlPath.c_str()) != 0)
	// 	std::cerr << RED << "Error remoiving tmp error page" << RESET <<  std:: endl; 
	setResponse(conn, response);
}

//todo: gaat niet goed. blijft de timeout request zorgt voor een segfault
// void	setErrorResponse(connection *conn, int error)
// {
// 	//**testprint*
// 	std::cout << "\nin setErroResponse with error: " << error << std::endl;
	
// 	HttpRequest		request(conn->request, conn->server);
// 	HttpResponse	response = HttpResponse();

// 	std::string		errorHtmlPath;
// 	std::string		host			= request.uri.getHost();
// 	std::string		location		= request.uri.getPath();
// 	std::cout << "errorHtmlPath: '" << errorHtmlPath << "' host: '" << host << "' location: '" << "'" << std::endl;
// 	std::string		root 			= conn->server->get_rootFolder(host, location);
	
// 	response.setStatusCode(error);

// 	// check for error pages set in config
// 	std::map<int, std::string> *providedErrorPages = conn->server->get_errorPages(host);
// 	if (providedErrorPages->size() != 0) {
// 		for (const auto& pair : *providedErrorPages) {
// 			if (pair.first == error) {
// 				std::cout << BLUE << "directed to error page set in config with nr: " << error <<  RESET << std::endl;
// 				errorHtmlPath = root + "/text/html" + pair.second;
// 				break;
// 			}
// 			else
// 				errorHtmlPath = generateErrorPage(error);
// 		}
// 	}
// 	else
// 		errorHtmlPath = generateErrorPage(error);

// 	if (error == 408 || error == 429 || error == 500 || error == 504) {
// 		conn->close_after_response = 1;
// 		response.headers->setHeader("Connection", "close");		
// 	}
	
// 	//check if path is ok
// 	std::ifstream f(errorHtmlPath);
// 	if (f.good())
// 		response.reSetBody(errorHtmlPath, false);

// 	setResponse(conn, response);
// }


void	setResponse(connection *conn, HttpResponse resp)
{
	conn->response = resp.serializeResponse();
	conn->request.clear();
	conn->state = WRITING;
}


// Some browsers (eg Firefox), check form timeout based on header keep-alive with timeout.
// others (eg safari) do not close the connection themselves
int	checkTimeout(connection *conn)
{
	// smaller timeout value for internal timeouts?
	double timeout;

	if (conn->state == IN_CGI)
		timeout = CGI_TIMEOUT;
	else
		timeout = conn->server->get_timeout();
	if (std::difftime(std::time(nullptr), conn->time_last_request) >= timeout) {
		switch(conn->state)
		{
			case READING:
				std::cerr << CYAN << "request timeout" << RESET << std::endl;
				setErrorResponse(conn, 408);
				return 1;
			case IN_CGI:
				std::cerr << CYAN << "timeout in cgi" << RESET << std::endl;
				if (conn->cgiPID) {
					close(conn->cgiPID);
					kill(conn->cgiPID, SIGTERM);
				}
				setErrorResponse(conn, 504);
				return 1;
			case HANDLING:
				std::cerr << CYAN << "timout in process" << RESET << std::endl;
				setErrorResponse(conn, 500);
				return 1;
			default:
				return 0;
		}
	}
	return 0;
}


std::string	removeWhitespaces(std::string str) {
	size_t	index;
	index = str.find_first_not_of(WHITE_SPACE);
	if (index != std::string::npos)
	 	str	= str.substr(index);
	index = str.find_last_not_of(WHITE_SPACE);
	if (index != std::string::npos)
	 	str	= str.substr(0, index + 1);
	return str;
}


void handleSignal(int signal) {
	g_shutdown_flag = signal;
}
