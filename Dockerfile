FROM alpine:latest as builder

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
	g++ \
	make

COPY ./src /usr/webserv/src/

COPY Makefile /usr/webserv/Makefile

WORKDIR /usr/webserv

RUN make

FROM alpine:latest

RUN apk update \
	&& apk upgrade \
	&& apk add --no-cache \
		libstdc++ \
		libgcc \
		python3 \
		php-cgi \
		bash

RUN ln -s /bin/bash /usr/bin/bash

COPY --from=builder /usr/webserv/webserv /usr/webserv/webserv

COPY ./config_files /usr/webserv/config_files

WORKDIR /usr/webserv/

ENTRYPOINT ["/usr/webserv/webserv", "/usr/webserv/config_files/default.conf"]