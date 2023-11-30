/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/22 21:57:55 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/30 15:07:15 by cwesseli      ########   odam.nl         */
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
	HttpRequest request(conn->request, conn->server);
	std::string errorHtmlPath;
	std::string host = request.getHeaderValue("host");
	
	// check for error pages set in config
	std::map<int, std::string> *providedErrorPages = conn->server->get_errorPages(host);
	if (providedErrorPages->size() != 0) {
		for (const auto& pair : *providedErrorPages) {
			if (pair.first == error) {
				std::cout << BLUE << "directed to error page set in config with nr: " << error <<  RESET << std::endl;
		
				errorHtmlPath = "./data/text/html" + pair.second; //todo: remove
				// errorHtmlPath = request.getRoot() + "/text/html" + pair.second;
				errorHtmlPath = conn->server->get_rootFolder(host, request.uri.getPath()) + "/text/html" + pair.second;

				std::cout << BLUE << "errorHtmlPath: " << errorHtmlPath <<  RESET << std::endl;
			}
		}
	}
	else 
		errorHtmlPath = generateErrorPage(error);
	
	HttpResponse response(request);
	response.setStatusCode(error);
	if (error == 408 || error == 429 || error == 500 || error == 504) {
		conn->close_after_response = 1;
		response.setHeader("Connection", "close");		
	}
	// std::string errorHtmlPath = generateErrorPage(conn, error); //for confid error page

	//check if path is ok
	std::ifstream f(errorHtmlPath);
	if (f.good())
		response.setBody(errorHtmlPath, false);
	setResponse(conn, response);
}


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
