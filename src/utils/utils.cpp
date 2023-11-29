/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/22 21:57:55 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/29 12:08:08 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

#include<string>
#include<fstream>


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
	std::string		errorHtmlPath = "";
	HttpRequest		request(conn->request, conn->server);
	HttpResponse	response(request);
	
	response.setStatusCode(error);
	
	// check for error pages set in config
	std::map<int, std::string> *providedErrorPages = conn->server->get_errorPages(request.getHeaderValue("host"));
	if (providedErrorPages->size() != 0) {
		for (const auto& pair : *providedErrorPages) {
			if (pair.first == error) {
				//**test
				std::cout << BLUE << "directed to error page set in config with nr: " << error <<  RESET << std::endl;
				errorHtmlPath = "data/text/html" + pair.second; //todo from root
			}
		}
	}
	
	//check for error 404 and check for dir listing is true >>
	if (errorHtmlPath.empty()) {
		std::cout << BLUE << "dirlist: " <<  request.getDirListing() <<  RESET << std::endl;
		if (error == 404 && request.getDirListing() == true)

			std::cout << RED << "Need to add code here" << RESET << std::endl; //todo: add script
		else	
			errorHtmlPath = generateErrorPage(error);
	}
		
	std::ifstream f(errorHtmlPath);
	if (f.good())
		response.setBody(errorHtmlPath, false);
	// if (std::remove(errorHtmlPath.c_str()) != 0) //todo uncomment to remove error pages
	// 	std::cerr << RED << "Error remoiving tmp error page" << RESET <<  std:: endl; 
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
void	checkTimeout(connection *conn)
{
	if (conn->request.empty()) 
	{
		// std::cout << "checkout timeout: " << difftime(std::time(nullptr), conn->time_last_request) << std::endl;
		if (difftime(std::time(nullptr), conn->time_last_request) > conn->server->get_timeout()) {

			std::cout << CYAN << "Timeout" << RESET << std::endl;
			conn->state = CLOSING; 
		}
		else
			conn->state = READING;
	}
	else
		conn->state = HANDLING;
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


void handleSignal(int signal)
{
	g_shutdown_flag = signal;
}
