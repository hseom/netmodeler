#PROGS = read_from_file sntime hstest rewiretest partdisttest makenet stattest weightedtest fitertest undist deetoo_test eventtest
PROGS = sntime genetictest_auto_group genetictest_auto_group_root genetictest_auto_group_new
NMDIR = ../src
LIBNM = $(NMDIR)/.libs/libnetmod.a

all : $(PROGS) 

%.o : %.cpp %.h
	g++ -c $<

% : %.cpp $(LIBNM)
	g++ -O2 -o $@ $< -I$(NMDIR) $(LIBNM) -lm
clean :
	rm $(PROGS)
