CC = gcc
CFLAGS =-I.
LIBS = -lbcm2835 -lpthread
OBJ = main.o server.o sensor.o
DEPS = server.h sensor.h

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

webserver: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

clean:
	-rm -f *.o 
