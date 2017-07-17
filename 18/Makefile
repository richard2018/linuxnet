CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread
TARGET = shttpd
RM = rm -f 
OBJS = shttpd_parameters.o shttpd.o shttpd_worker.o shttpd_uri.o shttpd_request.o shttpd_method.o shttpd_mine.o shttpd_error.o
all:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
clean:
	$(RM) $(TARGET) $(OBJS)
