#!/bin/bash
rm *.o
rm libMika.a
gcc -c mikaPosClient.c -lcurl
make libfreetype
make libagg
make libmrss
make libaggfreetype
make libtinyxml
make libjpeg
make menu
./packArchive
