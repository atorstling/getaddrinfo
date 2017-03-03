from debian:jessie
RUN apt-get -y  update
RUN apt-get -y install clang build-essential python libgoogle-perftools-dev\
 valgrind bash libbsd-dev
# Test deps
RUN apt-get -y install bsdmainutils
COPY . getaddrinfo
WORKDIR getaddrinfo
RUN make compile
ENV SHELL /bin/bash
