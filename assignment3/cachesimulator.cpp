/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <math.h>
#include <string.h>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {

      }
*/
class Cache {
private:
    int level;
    Cache* next;
    int blockSize;
    int size;
    int type;
    int setSize;
    int setCount;
    int* ptrs;
    int** mapping;
    int s, b, t;
public:

    Cache(int level, int blockSize, int setSize, int size) {
        this->level = level;
        this->blockSize = blockSize;
        this->size = size;
        if (setSize == 0) type = 0; // fully associativity
        else type = 1; // directed mapped and set associativity
        this->setSize = setSize;
        setCount = ((size * 1024) / setSize) / blockSize;
        std::cout << setCount << '\n';
        s = log(setCount) / log(2);
        b = log(blockSize) / log(2);
        t = 32 - s - b;
        ptrs = new int[setCount];
        mapping = new int* [setCount];
        for (size_t i = 0; i < setCount; i++) {
            mapping[i] = new int[setSize]();
            memset(mapping[i], -1, setSize * sizeof(int));
        }
    }

    void setNextLevel(Cache* l2) {
        next = l2;
    }

    bool read(bitset<32> address) {
        string str = address.to_string();
        bitset<32> temp1 (str.substr(0, t));
        int tag = (int) temp1.to_ulong();
        bitset<32> temp2 (str.substr(t, t + s));
        int index = (int) temp2.to_ulong();
        bitset<32> temp3 (str.substr(32 - b, 32));
        int offset = (int) temp3.to_ulong();
        for (size_t i = 0; i < setSize; i++) {
            if (mapping[index][i] == tag) {
                if (level == 1) {
                    return true;
                } else {
                    // if l2 cache, clean the block, because l1, l2 are non-inclusive
                    mapping[index][i] = -1;
                    return true;
                }
            }
        }
        //if l1 cache, storge the value
        if (level == 1) {
            bool isFilled = true;
            for (size_t i = 0; i < setSize; i++) {
                if (mapping[index][i] == -1) {
                    mapping[index][i] = tag;
                    isFilled = false;
                    break;
                }
            }
            if (isFilled == true) {
                if (ptrs[index] < setCount) {
                    mapping[index][ptrs[index]] = tag;
                    ptrs[index]++;
                } else {
                    mapping[index][0] = tag;
                    ptrs[index] = 0;
                }
                next->add(address);
            }
        }
        else {

        }
        return false;
    }

    bool write(bitset<32> address) {
        string str = address.to_string();
        bitset<32> temp1 (str.substr(0, t));
        int tag = (int) temp1.to_ulong();
        bitset<32> temp2 (str.substr(t, t + s));
        int index = (int) temp2.to_ulong();
        bitset<32> temp3 (str.substr(32 - b, 32));
        int offset = (int) temp3.to_ulong();
        for (size_t i = 0; i < setSize; i++) {
            if (mapping[index][i] == tag) {
                return true;
            }
        }
        return false;
    }

    //handle non-inclusive situation for l2
    void add(bitset<32> address) {
        string str = address.to_string();
        bitset<32> temp1 (str.substr(0, t));
        int tag = (int) temp1.to_ulong();
        bitset<32> temp2 (str.substr(t, t + s));
        int index = (int) temp2.to_ulong();
        bitset<32> temp3 (str.substr(32 - b, 32));
        int offset = (int) temp3.to_ulong();
        bool isFilled = true;
        for (size_t i = 0; i < setSize; i++) {
            if (mapping[index][i] == -1) {
                mapping[index][i] = tag;
                isFilled = false;
                break;
            }
        }
        if (isFilled == true) {
            if (ptrs[index] < setCount) {
                mapping[index][ptrs[index]] = tag;
                ptrs[index]++;
            } else {
                mapping[index][0] = tag;
                ptrs[index] = 0;
            }
        }
    }
};

int main(int argc, char* argv[]){

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L2blocksize;
      cache_params>>cacheconfig.L2setsize;
      cache_params>>cacheconfig.L2size;
      }

    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    Cache* l1 = new Cache(1, cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size);
    Cache* l2 = new Cache(2, cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);
    l1->setNextLevel(l2);

    int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;

    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";

    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;

    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache

            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);

            L1AcceState = NA;
            L2AcceState = NA;

           // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R")==0)
            {
                //Implement by you:
                //read access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;
                if (l1->read(accessaddr)) {
                    L1AcceState = RH;
                    L2AcceState = NA;
                } else {
                    L1AcceState = RM;
                    if (l2->read(accessaddr)) {
                        L2AcceState = RH;
                    } else {
                        L2AcceState = RM;
                    }
                }
            }
            else
            {
                //Implement by you:
                //write access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;
                if (l1->write(accessaddr)) {
                    L1AcceState = WH;
                    L2AcceState = NA;
                } else {
                    L1AcceState = WM;
                    if (l2->write(accessaddr)) {
                        L2AcceState = WH;
                    } else {
                        L2AcceState = WM;
                    }
                }
            }



            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;


        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";







    return 0;
}
