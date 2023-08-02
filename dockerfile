FROM ubuntu:20.04

RUN apt-get update -y
RUN apt-get install gcc -y
RUN apt-get install make -y
RUN apt-get install g++ -y

ADD . /arcmatch/
RUN cd /arcmatch && bash compile.sh