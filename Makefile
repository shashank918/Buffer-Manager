CC=gcc
CFLAGS=-I.
DEPS =

TestAssn2: test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	gcc -o TestAssn2 test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o -lm -g

test_assign2_1.o: test_assign2_1.c dberror.h storage_mgr.h test_helper.h buffer_mgr.h buffer_mgr_stat.h
	gcc -c test_assign2_1.c -lm -g

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h buffer_mgr.h
	gcc -c buffer_mgr_stat.c -g

buffer_mgr.o: buffer_mgr.c buffer_mgr.h dt.h storage_mgr.h
	gcc -c buffer_mgr.c -g

storage_mgr.o: storage_mgr.c storage_mgr.h 
	gcc -c storage_mgr.c -lm -g

dberror.o: dberror.c dberror.h 
	gcc -c dberror.c -g

clean:	
	rm -f *.o TestAssn2



