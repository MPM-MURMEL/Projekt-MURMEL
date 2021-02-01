#sudo apt install g++
#sudo apt-get install python-dev
#sudo apt install swig

# generate swig wrapper file
swig -python -c++ -I../src KinovaJaco2.i

# compile c++ with swig wrapper
g++ -Wall -fPIC -c ../src/KinovaJaco2*.c* ./KinovaJaco2*.c* -I/usr/include/python2.7 -I/usr/include -I../include -I../src -L/usr/lib -L../lib -lpython2.7 -ldl

# link object files to shared object file
g++ -shared -fPIC KinovaJaco2*.o -o _KinovaJaco2.so 
