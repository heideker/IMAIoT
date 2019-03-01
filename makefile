# for raspbian, include libcurl4-openssl-dev package
all: 
	gcc -Wall -o imaiot imaiot.cpp -lstdc++ -lcurl -lpthread -std=c++11

