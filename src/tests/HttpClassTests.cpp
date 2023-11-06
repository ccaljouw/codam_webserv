/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpClassTests.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:29:48 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 11:08:19 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Uri.h"

#include <cassert>
#include <iostream>

void testRequest(void) {

    // Example HTTP request strings
    std::string request1 = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\nThis is the body of the request.";
    std::string request2 = "POST /api/data HTTP/1.1\r\nUser-Agent: MyClient\r\nContent-Length: 15\r\n\r\nRequest body data";

    // Create HttpRequest objects
    HttpRequest httpRequest1(request1);
    HttpRequest httpRequest2(request2);

    // Display information from the first HTTP request
    std::cout << "Request 1 Method: " << httpRequest1.getMethod() << std::endl;
    std::cout << "Request 1 Protocol: " << httpRequest1.getProtocol() << std::endl;
    std::cout << "Request 1 URI: " << httpRequest1.getUri() << std::endl;
    std::cout << "Request 1 Headers:" << std::endl;
    for (const auto& header : httpRequest1.getHeaders()) {
        std::cout << header.first << ": " << header.second << std::endl;
    }
    std::cout << "Request 1 Body: " << httpRequest1.getBody() << std::endl;

    // Display information from the second HTTP request
    std::cout << "Request 2 Method: " << httpRequest2.getMethod() << std::endl;
    std::cout << "Request 2 Protocol: " << httpRequest2.getProtocol() << std::endl;
    std::cout << "Request 2 URI: " << httpRequest2.getUri() << std::endl;
    std::cout << "Request 2 Headers:" << std::endl;
    for (const auto& header : httpRequest2.getHeaders()) {
        std::cout << header.first << ": " << header.second << std::endl;
    }
    std::cout << "Request 2 Body: " << httpRequest2.getBody() << std::endl;

}


void testUri(void) {
	
    std::string uriString = "http://example.com:8080/path/to/resource?query=value#fragment";

    // Create a Uri object
    Uri uri(uriString);

    // Display information from the parsed URI
    std::cout << "Scheme: " << uri.getScheme() << std::endl;
    std::cout << "Authority: " << uri.getAuthority() << std::endl;
    std::cout << "Path: " << uri.getPath() << std::endl;
    std::cout << "Query: " << uri.getQuery() << std::endl;
    std::cout << "Fragment: " << uri.getFragment() << std::endl;
    std::cout << "Userinfo: " << uri.getUserInfo() << std::endl;
    std::cout << "Host: " << uri.getHost() << std::endl;
    std::cout << "Port: " << uri.getPort() << std::endl;

	std::cout << "Original URI: " << uriString << std::endl;
    std::cout << "Serialized URI: " << uri.serializeUri() << std::endl;
 }


void testHttpResponse() {
    // Create an HttpResponse object
    HttpResponse response;

    // Set properties
    response.setProtocol("HTTP/1.1");
    response.setStatusCode(200);
    response.addHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>Hello, World!</h1></body></html>");

    // Serialize the response
    std::string serializedResponse = response.serializeResponse();

    // Print the serialized response
    std::cout << "Serialized Response:\n" << serializedResponse << "\n";
}