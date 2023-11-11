#include<string>
#include<sstream>
#include<ctime>
#include "webServ.hpp"


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



// to do: make timeout check
void	checkTimeout(connection *conn)
{
	if (conn->request.empty()) // and timeout
		conn->state = CLOSING; 
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


//todo: more cookies than one	
std::string		checkAndSetCookie(connection* conn, HttpRequest& request) {

	std::string cookieValue		=	request.getHeaderValue("cookie"); //returns the value part of the cookie

	std::string	cookieName;
	std::string	cookieId;
	std::string	cookieTrigger = "placeholder";

	//parse cookieValue
	if(!cookieValue.empty())
	{
		std::istringstream	cookieStream(cookieValue);
		std::string			keyValuePair;

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
	}
	//todo: handle trigger
	if (cookieName == "webserv42") //todo:get rid of hard code
	{
		for (auto& pair : conn->knownClientIds)
		{
			if (pair.first == cookieId)
			{
				pair.second += 1;
			
				std::cout << "identified existing user_id :"<< cookieId << std::endl; 
				std::cout << "ID has visited us " << pair.second << " times!" << std::endl;
				std::cout << "ID trigger value = '" << cookieTrigger << "'" << std::endl; //todo: remove lines

				std::string newCookieValue = "name=webserv42, id=" + cookieId + ", trigger=" + cookieTrigger;
				return newCookieValue;
			}
		}
	}

	//set cookie for empty and unknow client using current time-hash
	std::hash<std::time_t>		timeHash;
	std::time_t now			=	std::time(nullptr);
	size_t hashValue		=	timeHash(now);

	std::string newCookieId = std::to_string(hashValue);
	conn->knownClientIds.insert(std::make_pair(newCookieId, 1));
//	std::cout << "added to know ids: " << newCookieId << std::endl;
	std::string newCookieValue = "name=webserv42, id=" + newCookieId + ", trigger=" + cookieTrigger;
	return newCookieValue;
}
