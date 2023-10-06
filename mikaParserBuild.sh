# needs posix threads and BerkeleyDB
#
#!/bin/bash
gcc -c mikaParser.c
gcc -c mikaParserFilter.c
gcc -c mikaParserEmailFilter.c
gcc -c mikaParserIterator.c
#gcc -o mikaParser mikaParser.o mikaParserIterator.o -lpthread -ldb
gcc -o mikaParserFilter mikaParserFilter.o mikaParser.o mikaParserIterator.o -lpthread -ldb
#gcc -o mikaParserEmailFilter mikaParserEmailFilter.o mikaParser.o -lpthread -ldb
