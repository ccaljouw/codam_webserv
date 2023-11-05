/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HttpClassTests.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:29:48 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/05 20:52:26 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Uri.h"

#include <cassert>
#include <iostream>

void testRequest(void) {
    try {
        // Sample HTTP request string
        std::string httpRequestString = "GET /index.html HTTP/1.1\r\n"
                                       "Host: www.example.com\r\n"
                                       "User-Agent: MyHttpClient\r\n"
                                       "Accept-Language: en-US, en;q=0.5\r\n"
                                       "Accept-Encoding: gzip, deflate\r\n"
                                       "\r\n"
                                       "This is the request body.";

        // Create HttpRequest object
        HttpRequest request(httpRequestString);

        // Display the contents the object
        std::cout << "Request Method: " << request.getMethod() << std::endl;
        std::cout << "Request Protocol: " << request.getProtocol() << std::endl;
        std::cout << "Request URI: " << request.getUri() << std::endl;
        std::cout << "Request Body: " << request.getBody() << std::endl;

        // Serialize and display the headers
        std::string serializedHeaders = request.serializeHeaders();
        std::cout << "Serialized Headers:" << std::endl << serializedHeaders;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void testUri(void) {

    // Test 1: Valid URI Serialization
    Uri uri1("https://www.example.com:8080/path/to/resource?query=value#fragment");
    std::string serialized1 = uri1.serializeUri();
    assert(serialized1 == "https://www.example.com:8080/path/to/resource?query=value#fragment");
    std::cout << "Test 1 (Valid URI Serialization) passed." << std::endl;

    // Test 2: Empty URI Serialization
    Uri uri2;
    std::string serialized2 = uri2.serializeUri();
    assert(serialized2.empty());
    std::cout << "Test 2 (Empty URI Serialization) passed." << std::endl;

    // Test 3: Authority Splitting
    Uri uri3("https://user:password@www.example.com:8080/path/to/resource?query=value#fragment");
    assert(uri3.getAuthority() == "user:password@www.example.com:8080");
    assert(uri3.getUserInfo() == "user:password");
    assert(uri3.getHost() == "www.example.com");
    assert(uri3.getPort() == 8080);
    std::cout << "Test 3 (Authority Splitting) passed." << std::endl;


    std::cout << "All tests passed." << std::endl;
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