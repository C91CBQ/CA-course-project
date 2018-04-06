Options:

1)From an IDE:
Import cachesimulator.cpp, trace.txt and cacheconfig.txt to CLion or any IDE configured for C/C++.
For Run configuration: Specify the folder in which both .txt files are(Working directory). 
Enter both .txt file names as program arguments next to each other like this: cacheconfig.txt trace.txt.
Then you should be able to run it.

2)From the Command line
You can run the command:

gcc cachesimulator.cpp cacheconfig.txt trace.txt -o

			OR 

g++ cachesimulator.cpp cacheconfig.txt trace.txt -o


