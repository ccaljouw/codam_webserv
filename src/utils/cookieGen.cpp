/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cookieGen.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/24 08:20:32 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/24 14:05:58 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"


float	generateRandomFloat(float fmin, float fmax) {
	float	a = fmin + static_cast<float>(std::rand());
	float	b = static_cast<float>(RAND_MAX / (fmax - fmin));
	return a / b;
}


//todo: handle cookie trigger?
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