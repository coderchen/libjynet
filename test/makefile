test_webserver=webserver
test_timer=timer
test_timer2=timer2

all:$(test_webserver) \
		$(test_timer) \
		$(test_timer2)

$(test_webserver):webserver.o
	g++ -o $@ $< -ljynet

$(test_timer):timer.o
	g++ -o $@ $< -ljynet

$(test_timer2):timer2.o
	g++ -o $@  $< -ljynet

%.o:%.cpp
	g++ -Wall -c $< -o $@

.PHONY:clean
clean:
	-rm -f $(test_webserver) \
				 $(test_timer) \
				 $(test_timer2) \
				 *.o
