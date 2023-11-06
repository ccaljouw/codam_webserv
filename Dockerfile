FROM ubuntu:latest

ENV CC="c++" CFLAGS="-Wall -Wextra -Werror"

RUN apt-get -y update \
&& \
	apt-get install \
	gcc \
	g++ \
	vim \
	make \
	git \
	lldb \
	valgrind \
	libcriterion-dev \
	libreadline-dev \
	strace \
	manpages \
	less \
	-y \
&& \
	apt-get clean \
&& \
	rm -rf /var/lib/apt/lists/*


#	
#	start up docker application
#
#	to build image:
#	docker build ./ -f Dockerfile
#
#	to find image_ID:
#	docker image list
#
#	to run docker container:
#	docker run -it --rm -v $PWD:/[XXX] -p [PORT:PORT] [image_tag/image_ID]
#	docker run -it --rm -v $PWD:/webs -p 8080:8080  7761f0be951a
#
#	cd [XXX]
#
#	to run valgrind:
#	valgrind --leak-check=full ./webserv (OPTIONIAL: --show-leak-kinds=all)
#	valgrind --leak-check=full --trace-children=yes ./webserv
#	leaks shown at exit (or ctrl-D)
