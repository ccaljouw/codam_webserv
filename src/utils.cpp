/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/22 21:57:55 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/22 21:57:58 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

#include<string>
#include<sstream>
#include<ctime>
#include<cmath>

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

void	setErrorResponse(connection *conn, int error)
{
	HttpResponse response;
	response.setStatusCode(error);
	setResponse(conn, response);
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

//todo: handle trigger
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
