src=$(wildcard *.cpp)
obj=$(patsubst %.cpp, %.o, $(src))
cc=g++
target=libjynet.a

$(target):$(obj)
	ar -r $(target) $(obj)

$(obj):%.o:%.cpp
	$(cc) -Wall -c -o $@ $<

.PHONY:clean cleanobj tag install
clean:
	-rm -f $(obj) $(target)
cleanobj:
	-rm -f $(obj)
install:
	rm -rf /usr/local/include/jynet
	mkdir -p /usr/local/include/jynet
	cp *.h /usr/local/include/jynet
	cp $(target) /usr/local/lib
tag:
	ctags -R *
