#! /bin/bash
cd messages
make clean; make; make install
cd ..
cd services
make clean; make; make install
cd ..
cd core
make clean; make; make install
cd ..
cd contrib
make clean; make; make install
cd ..
cd examples
make clean; make; make install
cd ..
cd test
make clean; make; make install
cd ..
cd ../contrib/LANL
make clean; make; make install
cd ../..
