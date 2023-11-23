/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/22 21:57:55 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/23 11:34:50 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

#include<string>
#include<sstream>
#include<ctime>
#include<cmath>
#include <fstream>
#include <sys/stat.h>

// Success 2xx /  Error 4xx, 5xx / Redirection 3xx
//todo: move to config server settings 
std::map<int, std::string> errorPages = {
 	{ 400, "Bad request"},					// The request had bad syntax or was inherently impossible to be satisfied. 
 	// { 401, "Unauthorized"},					// The parameter to this message gives a specification of authorization schemes which are acceptable. The client should retry the request with a suitable Authorization header. 
 	{ 403, "Forbidden"},					// The request is for something forbidden. Authorization will not help. 
	{ 404, "Not found"},					//  The server has not found anything matching the URI given 
	{ 405, "Method Not Allowed"},
	{ 422, "Unprocessable Entity"}, 		// Indicates that the server understands the content type of the request entity, and the syntax of the request entity is correct, but it was unable to process the contained instructions. 
	{ 500, "Internal Error"},				// The server encountered an unexpected condition which prevented it from fulfilling the request. 
	{ 501, "Not implemented"},				// The server does not support the facility required. 
	// { 502, "Service temporary overloaded"},
	// { 503, "Gateway timeout"},
	{ 505, "version not supported"}
};


float generateRandomFloat(float fmin, float fmax) {
	float	a = fmin + static_cast<float>(std::rand());
	float	b = static_cast<float>(RAND_MAX / (fmax - fmin));
	return a / b;
}

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
//checks is a directory exists
bool	directoryExists(const std::string& path) {
	struct stat info;
	return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR) != 0;
}

//todo: code for config erro pages below 
// std::string	generateErrorPage(connection *conn, int e) {
// 	std::string error = std::to_string(e);
// 	std::string title = "Undefined Error";
// 	std::map<int, std::string> errorPages = conn->server->get_errorPages();
// 	for (const auto& pair : errorPages)
// 		if (pair.first == e)
// 			title = pair.second;


std::string	generateErrorPage(int e) {
	std::string error = std::to_string(e);
	std::string title = "Undefined Error";
	for (const auto& pair : errorPages)
		if (pair.first == e)
			title = pair.second;

//set page content
	std::string htmlContent = R"(
<!DOCTYPE html>
<html>
<head>
	<title>)" + error + " " + title + R"(</title>
</head>
<body>
	<h1>)" + error + " " + title + R"(</h1>
</body>
</html>
)";
	//create tmp dir
	std::string pathToTmp = "tmp";
	if (!directoryExists(pathToTmp)) {
		if (mkdir(pathToTmp.c_str(), 0777) == -1) 
			std::cerr << RED << "Error making dir" << RESET << std::endl;
	}

	//write to html page
	std::string path = pathToTmp + "/" + error + ".html";
	std::ofstream errorPageStream(path);
	if (errorPageStream.is_open()) {
		errorPageStream << htmlContent;
		errorPageStream.close();
	}
	else
		std::cerr << RED <<  "unable to generate error HTML file" << RESET <<  std::endl;
	return path;
}

void	setErrorResponse(connection *conn, int error)
{
	HttpResponse response;
	response.setStatusCode(error);
	std::string errorHtmlPath = generateErrorPage(error);
	// std::string errorHtmlPath = generateErrorPage(conn, error); //for confid error page

	std::ifstream f(errorHtmlPath);
	if (f.good())
		response.setBody(errorHtmlPath, false);
	//todo : remove tmp file or not
	// if (std::remove(errorHtmlPath.c_str()) != 0) //todo uncomment
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
		if (difftime(std::time(nullptr), conn->time_last_request) > conn->server->get_timeout("servername")) {

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

//todo: handle trigger?
std::string		checkAndSetCookie(connection* conn, HttpRequest& request) {

	std::string	cookieName;
	std::string	cookieId;
	std::string	cookieTrigger = "placeholder";

	std::string cookieValue	= request.getHeaderValue("cookie"); //returns the value part of the cookie

	size_t ourCookieStart = cookieValue.find("name=" + std::string(HOST));
	if (ourCookieStart != 0 && ourCookieStart != std::string::npos)
		cookieValue = cookieValue.substr(ourCookieStart);

	std::istringstream	cookieStream(cookieValue);
	std::string			keyValuePair;

	//parse cookieValue
	while(std::getline(cookieStream, keyValuePair, ','))
	{
		size_t equalPos = keyValuePair.find('=');
		if (equalPos != std::string::npos)
		{
			std::string key		=	keyValuePair.substr(0, equalPos);
			std::string value	=	keyValuePair.substr(equalPos + 1);

			key = removeWhitespaces(key);
			value = removeWhitespaces(value);

			if (key == "name")
				cookieName = value;
			else if (key == "id")
				cookieId = value;
			else if (key == "trigger")
				cookieTrigger = value;
		}
	}

	//check known id and if found set int +1 and handle trigger
	if (!conn->server->checkClientId(cookieId)) {
		//set cookie for empty and unknow client using current time-hash * random number
		std::hash<std::time_t>		timeHash;
		std::time_t now			=	std::time(nullptr);
		size_t hashValue		=	timeHash(now);

		float id = hashValue * generateRandomFloat(0.0, 500.0);
		cookieId = std::to_string(static_cast<long int>(id));
		conn->server->addClientId(cookieId);
	}
	std::string newCookieValue = "name=" + std::string(HOST) + ", id=" + cookieId + ", trigger=" + cookieTrigger;
	return newCookieValue;
}

void handleSignal(int signal)
{
	g_shutdown_flag = signal;
}
