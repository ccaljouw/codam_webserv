/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webServ.h                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:57:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/03 17:16:48 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#ifndef WEBSERV_H
# define WEBSERV_H

#define LINE_END "\r\n"
#define WHITE_SPACE " \t\n\v\f\r"
#define HTTP_PROTOCOL "HTTP/1.1"

enum eventStatus {
	WAITING,
	READING,
	WRITING
};


#endif

