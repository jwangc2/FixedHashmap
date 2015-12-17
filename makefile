all: hashmapTester

hashmapTester: hashmapTester.o FixedHashmap.o
	g++ hashmapTester.o FixedHashmap.o -o hashmapTester

hashmapTester.o: hashmapTester.cc
	g++ -c hashmapTester.cc

FixedHashmap.o: FixedHashmap.cc FixedHashmap.h
	g++ -c FixedHashmap.cc

clean:
	rm hashmapTester.o FixedHashmap.o
