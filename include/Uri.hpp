/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Uri.h                                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 12:12:15 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 10:06:50 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef URI_H
# define URI_H

#include <string>

class Uri {

	public:
		Uri(void);
		Uri(const std::string& uri);
		Uri(const Uri& origin);
		const Uri& operator=(const Uri& rhs);

		std::string		serializeUri(void);

		// ========== Getters ===========
		std::string		getScheme(void) const;
		std::string		getAuthority(void) const;
		std::string		getPath(void) const;
		std::string		getQuery(void) const;
		std::string		getFragment(void) const;
		std::string		getUserInfo(void) const;
		std::string		getHost(void) const;
		int				getPort(void) const;

	private:
		void			splitAuthority(void);
		
		std::string		_scheme;
		std::string		_authority;
		std::string		_path;
		std::string		_query;
		std::string		_fragment;
		std::string		_userinfo;
		std::string		_host;
		int				_port;
};

#endif