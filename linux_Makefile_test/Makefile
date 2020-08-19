CC = gcc 
OBJS = main.o calculate.o input.o
TARGET = cal
TARGET_2 = cal_2
CLAGS = -g
CA = libcalc

all : $(CA).a $(CA) $(TARGET) $(TARGET_2)

$(CA): calculate.o input.o
	$(CC) -shared -Wl,-soname,$(CA).so.0 -o $(CA).so.0.0.0 input.o calculate.o
	ln -s $(CA).so.0.0.0 $(CA).so
	ln -s $(CA).so.0.0.0 $(CA).so.0
	ln -s $(CA).so.0.0.0 $(CA).so.0.0


$(TARGET) : $(OBJS) 
	$(CC) -o $@ $< -L. -lcalc   
	
$(TARGET_2) : $(OBJS)
	$(CC) -o $@ $^

$(CA).a : calculate.o input.o
	ar rc $(CA).a calculate.o input.o

input.o : input.c
	$(CC) -fPIC -c $^

calculate.o : calculate.c
	$(CC) -fPIC -c $^

#.c.o: 
#	$(CC) -c -o $@ $<

clean: 
	rm -f *.o *.a *.so.* *.so $(OBJS) $(TARGET) $(TARGET_2)
