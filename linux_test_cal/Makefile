cc=gcc

TARGET_SRV=op_server

OBJS_SRV=op_server2.o error.o cal.o

TARGET_CLN=op_client
OBJS_CLN = op_client2.o error.o cal.o

#LDFLAGS=-D_REENTRANT -pthread

all: $(TARGET_SRV) $(TARGET_CLN)

$(TARGET_SRV) : $(OBJS_SRV)
	$(cc) -o $@ $^

$(TARGET_CLN) : $(OBJS_CLN)
	$(cc) -o $@ $^

.c.o : 
	$(cc) -c -o $@ $<

clean :
	rm -f *.o $(TARGET_SRV) $(TARGET_CLN)

