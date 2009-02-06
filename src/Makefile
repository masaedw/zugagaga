# last modified <2004/02/14 20:30:42 2004 JST>
.PHONY: clean all objs

ostype		= $(shell uname -s)

CXX		= g++
SRCS		= $(wildcard *.cpp)
OBJS		= $(SRCS:.cpp=.o)
DEPS		= $(SRCS:.cpp=.dep)
LIBS		= $(shell sdl-config --libs)
CLIENTLIBS	= $(LIBS) -lSDL_image -lSDL_ttf -lSDL_net
ZU3GA		= ../zu3ga

INCFLAG		= -I../include $(shell sdl-config --cflags)
ifdef BOOST_DIR
  INCFLAG	+= -I$(BOOST_DIR)
endif

ifeq ($(ostype), Darwin)
  CFLAGS	= -W -Wall -ansi $(INCFLAG) -Wno-long-double
else
  ifeq ($(findstring CYGWIN,$(ostype)), CYGWIN)
    BIN_SUFFIX	= .exe
    CXX		+= -mno-cygwin
    CFLAGS	= -W -Wall $(INCFLAG)
  else
    CFLAGS	= -W -Wall -ftemplate-depth-100 $(INCFLAG)
  endif
endif

CLIENTOBJS 	= \
		client_main.o\
		clientapp.o\
		clientapp_guiinit.o\
		clientapp_indep.o\
		draw.o\
		effect.o\
		event.o\
		eventmanager.o\
		graphic.o\
		key_command.o\
		localeio.o\
		localio.o\
		network.o\
		networkio.o\
		object.o\
		objects.o\
		planex.o\
		player.o\
		shell.o\
		utility.o\
		verbose.o\
		yamaha.o\
		ntt.o

OBJTESTOBJS	= \
		draw.o\
		effect.o\
		event.o\
		eventmanager.o\
		graphic.o\
		key_command.o\
		object.o\
		objects.o\
		objtest_main.o\
		planex.o\
		player.o\
		shell.o\
		utility.o\
		verbose.o\
		yamaha.o\
		ntt.o

all: $(ZU3GA)$(BIN_SUFFIX)

objs: $(OBJS)

$(ZU3GA)$(BIN_SUFFIX): $(CLIENTOBJS)
	$(CXX) $^ $(CLIENTLIBS) -o $@
	strip $@

objtest$(BIN_SUFFIX): $(OBJTESTOBJS)
	$(CXX) $^ $(CLIENTLIBS) -o $@

%.o: %.cpp
	$(CXX) -c -O3 $(CFLAGS) $<

draw.o: draw.cpp
	$(CXX) -c $(CFLAGS) $<

graphic.o: graphic.cpp
	$(CXX) -c $(CFLAGS) $<

%.dep: %.cpp
	$(SHELL) -ec '$(CXX) -MM $(INCFLAG) $< \
	  | sed "s/\($*\)\.o[ :]*/\1.o $@: /g" > $@; \
	  [ -s $@ ] || rm -f $@'

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

clean:
	rm -f $(OBJS) $(DEPS) $(ZU3GA)$(BIN_SUFFIX) objtest$(BIN_SUFFIX)
