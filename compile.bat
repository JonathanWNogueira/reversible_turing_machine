g++ -c rtm.cpp -o rtm.o
g++ -c main.cpp -o main.o
g++ main.o RTM.o -o main
.\main < input.txt

del main.o
del rtm.o
del main.exe