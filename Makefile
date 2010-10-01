CXX = g++
CFLAGS = -g -Wall -pedantic
PROD_CFLAGS = -O3 -fomit-frame-pointer -Wall
LDLIBS = -ltiff

GTK = `pkg-config --cflags --libs gtk+-2.0`

OBJS = 	image-basic.o \
		region-ops.o \
		boundary-ops.o \
		greylevel-ops.o \
		eval.o \
		statistic.o \
		curvature.o

GTK_OBJS = gtk-image.o

.SUFFIXES:	.cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(GTK) -c $*.cpp

all: pro-image pro-gtk-image

clean:
	rm *.o

backup: 
	tar -cvzf backcode.tgz *.cpp *.hpp

pro-image: $(OBJS) image-main.o
	$(CXX) $(CFLAGS) -o pro-image $(OBJS)  $(LDLIBS) image-main.o

pro-gtk-image: $(OBJS) $(GTK_OBJS) gtk-main.o
	$(CXX) $(CFLAGS) $(GTK) -o pro-gtk-image $(OBJS) $(GTK_OBJS) $(LDLIBS) gtk-main.o


