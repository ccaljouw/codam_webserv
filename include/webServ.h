/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webServ.h                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:57:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/05 20:47:43 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#ifndef WEBSERV_H
# define WEBSERV_H

#define LINE_END "\r\n"
#define WHITE_SPACE " \t\n\v\f\r"
#define HTTP_PROTOCOL "HTTP/1.1"

#include<map>
#include<string>


// Success 2xx /  Error 4xx, 5xx / Redirection 3xx 
// std::map<int, std::string> HttpStatusCodes = {
// 	{ 200, "OK"}, 					// The request was fulfilled. 
// 	{ 201, "Created"},				// Following a POST command, this indicates success, but the textual part of the response line indicates the URI by which the newly created document should be known. 
// 	{ 202, "Accepted"},				// The request has been accepted for processing, but the processing has not been completed.
// 	{ 204, "No response"},			// Server has received the request but there is no information to send back, and the client should stay in the same document view. 
//     { 400, "Bad request"},			// The request had bad syntax or was inherently impossible to be satisfied. 
//     { 401, "Unauthorized"},			// The parameter to this message gives a specification of authorization schemes which are acceptable. The client should retry the request with a suitable Authorization header. 
//     { 403, "Forbidden"},			// The request is for something forbidden. Authorization will not help. 
// 	{ 404, "Not found"},			//  The server has not found anything matching the URI given 
// 	{ 422, "Unprocessable Entity"}, // Indicates that the server understands the content type of the request entity, and the syntax of the request entity is correct, but it was unable to process the contained instructions. 
// 	{ 500, "Internal Error"},		// The server encountered an unexpected condition which prevented it from fulfilling the request. 
// 	{ 501, "Not implemented"},		// The server does not support the facility required. 
// 	{ 502, "Service temporary overloaded"},
// 	{ 503, "Gateway timeout"}
// };


// enum eventStatus {
// 	WAITING,
// 	READING,
// 	WRITING
// };


#endif

