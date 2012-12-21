COMPILE = g++ -c -v -g
#COMPILE = /depot/CenterLine/bin/CC -c -v -g

LINK = g++ -v
#LINK = /isd/depot/CenterLine/bin/CC -v

RHOME = /home/isd/kramer/reason

bin/bap: ofiles/bap.o
	$(LINK) -o bin/bap ofiles/bap.o

bin/bap_search1: ofiles/bap_search1.o
	$(LINK) -o bin/bap_search1 ofiles/bap_search1.o

bin/bap_search2: ofiles/bap_search2.o
	$(LINK) -o bin/bap_search2 ofiles/bap_search2.o

ofiles/bap.o: source/bap.c
	$(COMPILE) -o ofiles/bap.o source/bap.c

ofiles/bap_search1.o: source/bap_search1.c
	$(COMPILE) -o ofiles/bap_search1.o source/bap_search1.c

ofiles/bap_search2.o: source/bap_search2.c
	$(COMPILE) -o ofiles/bap_search2.o source/bap_search2.c

# stuff after this is for Centerline

load_bap:
	echo "Loading bap source into Centerline"
	#setopt program_name bap
	#load source/bap.c -lm

load_bap_search1:
	echo "Loading bap source into Centerline"
	#setopt program_name bap_search1
	#load source/bap_search1.c -lm

load_bap_search2:
	echo "Loading bap source into Centerline"
	#setopt program_name bap_search2
	#load source/bap_search2.c -lm

