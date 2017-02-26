from debian:jessie
RUN apt-get -y update
RUN apt-get -y install clang build-essential python libgoogle-perftools-dev\
 valgrind bash 
# Test deps
RUN apt-get -y install bsdmainutils
COPY . getaddrinfo
WORKDIR getaddrinfo
ENV SHELL /bin/bash
# Test files
COPY test/home /root
RUN make all
