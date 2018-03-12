#include<iostream>
#include<fstream>
#include<cmath>
#include<stdlib.h>
using namespace std;

void cntInc(int* counter){
	if(*counter<3) *counter=(*counter)+1;
}

void cntDec(int* counter){
	if(*counter>0) *counter=(*counter)-1;
}


int main(int argc,char* argv[]){
	if(argc!=3){
		cerr<<"Usage: ./predictor config.txt trace.txt\n"; 
	    return 0;
	    }
	    
	ifstream config(argv[1]);
    if (!config.is_open()) {
        cerr << "ERROR: The config file does not exist!\n";
        return 1;
    }

    ifstream trace(argv[2]);
    if (!trace.is_open()) {
        cerr << "ERROR: The trace file does not exist!\n";
        return 2;
    }

	ofstream outfile("./output.txt");
	

	int m;
	config>>dec>>m;
	long long table_size=pow(2,m);
	
	int* counter_table=new int[table_size];  //a 2^m 2-bit counter group; each counter can be represented using a integer in {0,1,2,3}
	
	
	for(int i=0;i<table_size;i++)
		//initialize the counter table to all '1' state
		counter_table[i]=1;
	
	int index=0;
	int prediction=0;
	int correctnum=0;
	double precision=0;
	
	/*while*/
	//1.read a line of trace file;
	//2.adjust counter;
	//3.make prediction;
	//4.collect precision num
	while(!trace.eof()){
		long long address;
		
		trace >> hex >> address;
        if (trace.fail()) break;

        char branchTaken[1]={};
        trace >> branchTaken;
        if (trace.fail()) break;

		precision=precision+1; //count the number of lines in trace file
        trace.ignore(256, '\n');
        
        index=address % table_size; //get the list significant m bits as the index to the counter table
        
        if(branchTaken[0]=='1') cntInc(counter_table+index);
        if(branchTaken[0]=='0') cntDec(counter_table+index); 
        
        if(counter_table[index]>=2) prediction=1;
        else prediction=0;
        
        outfile<<dec<<' '<<prediction<<'\n';
        
        if(prediction==atoi(branchTaken))
        	correctnum+=1;
	}	
	
	precision=correctnum/precision;
	cout<<precision<<endl;
	outfile.close();
	trace.close();
	config.close();
	return -1;
}
