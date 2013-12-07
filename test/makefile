test_webserver=webserver

all:$(test_webserver)

$(test_webserver):webserver.o
	g++ -o $(test_webserver) $< -ljynet

%.o:%.cpp
	g++ -Wall -c $< -o $@

.PHONY:clean
clean:
	-rm -f $(test_webserver) *.o
