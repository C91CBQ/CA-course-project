#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
    public:
        bitset<32> ReadData1, ReadData2;
     	RF()
    	{
          Registers.resize(32);
          Registers[0] = bitset<32> (0);
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
            if (WrtEnable.to_ulong() == 1) {
                Registers[WrtReg.to_ulong()] = WrtData;
                cout << "WrtData: " << WrtData << endl;
            } else {
                ReadData1 = Registers[RdReg1.to_ulong()];
                ReadData2 = Registers[RdReg2.to_ulong()];
            }
            // implement the function by you.
         }

	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {
                        rfout << Registers[j]<<endl;
                      }

                  }
                  else cout<<"Error from OutputRF: Unable to open file" <<endl <<endl;;
                  rfout.close();

               }
	private:
            vector<bitset<32> >Registers;

};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {
                 // implement the ALU operations by you.
                 if(ALUOP.to_ulong() == ADDU)//addition
                          ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
                      else if(ALUOP.to_ulong() == SUBU)//subtraction
                          ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
                      else if(ALUOP.to_ulong() == AND)//and
                          ALUresult = oprand1 & oprand2;
                      else if (ALUOP.to_ulong() == OR)//or
                          ALUresult = oprand1|oprand2;
                      else if (ALUOP.to_ulong() == NOR)//nor
                          ALUresult = ~(oprand1|oprand2);
                      return ALUresult;
               }
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize);
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open()) {

                      //cout<<"file not open"<<endl <<endl;
                  while (getline(imem,line))
                     {
                        IMem[i] = bitset<8>(line);
                         cout << "All Imem: " << IMem[i] << endl;
                         //cout << "line: " << line << endl;
                        i++;
                     }

                  }
                  else cout<<"Error from INSMem Constructor:Unable to open file"<<endl <<endl;
                  imem.close();

                  }

          bitset<32> ReadMemory (bitset<32> ReadAddress)
              {
                  Instruction=bitset<32>(IMem[ReadAddress.to_ulong()].to_string()+IMem[ReadAddress.to_ulong()+1].to_string()+IMem[ReadAddress.to_ulong()+2].to_string()+IMem[ReadAddress.to_ulong()+3].to_string());
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
                  //cout << "ReadMemory OK"<< endl;
                  cout << "Instruction: " << Instruction << endl;


                  return Instruction;
              }

      private:
           vector<bitset<8> > IMem;

};

class DataMem
{
      public:
          bitset<32> readdata;
          DataMem()
          {
             DMem.resize(MemSize);
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Error from DataMem Constructor: Unable to open file"<<endl <<endl;
                  dmem.close();

          }
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
          {
              if(readmem.to_ulong())
                  readdata=bitset<32>(DMem[Address.to_ulong()].to_string()+DMem[Address.to_ulong()+1].to_string()+DMem[Address.to_ulong()+2].to_string()+DMem[Address.to_ulong()+3].to_string());
              if(writemem.to_ulong()){
                  DMem[Address.to_ulong()]=bitset<8>(WriteData.to_string().substr(0,8));
                  DMem[Address.to_ulong()+1]=bitset<8>(WriteData.to_string().substr(8,8));
                  DMem[Address.to_ulong()+2]=bitset<8>(WriteData.to_string().substr(16,8));
                  DMem[Address.to_ulong()+3]=bitset<8>(WriteData.to_string().substr(24,8));
                 }
                 return readdata;
          }

          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {
                        dmemout << DMem[j]<<endl;
                       }

                  }
                  else cout<<"Error from OutputDataMem: Unable to open file"<<endl <<endl;
                  dmemout.close();

               }

      private:
           vector<bitset<8> > DMem;

};



int main() {
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

        bitset<32> curIns = 0xff;
        bitset<32> PC = 0x0;

        //R-type parts
        bitset<6> opcode = 0x0;
        bitset<5> rs = 0x0;
        bitset<5> rt = 0x0;
        bitset<5> rd = 0x0;
        bitset<5> shamt = 0x0;
        bitset<6> funct = 0x0;

        //I-type additional parts
        bitset<16> immediate = 0x0;

        //J-type additional parts
        bitset<26> address = 0x0;

        while (1) {
            bitset<32> immediate_final;
            // Fetch
            curIns = myInsMem.ReadMemory(PC);
            PC = bitset<32>(PC.to_ulong() + 4);
			cout<<"PC:"<<PC.to_ulong()<<endl;
            // If current insturciton is "11111111111111111111111111111111", then break;
            if (curIns == 0xffffffff) break;
            // decode(Read RF)
            opcode = bitset<6>(curIns.to_string().substr(0, 6));

			cout<<"opcode: "<<opcode<<endl;
            switch (opcode.to_ulong()) {
                case 0x00: {
                    //R-type
                    rs = bitset<5>(curIns.to_string().substr(6, 5));
                    rt = bitset<5>(curIns.to_string().substr(11, 5));
                    rd = bitset<5>(curIns.to_string().substr(16, 5));
                    shamt = bitset<5>(curIns.to_string().substr(21, 5));
                    funct = bitset<6>(curIns.to_string().substr(26, 6));
                    myRF.ReadWrite(rs, rt, rd, bitset<32>(0x0), 0);
                    myALU.ALUOperation(bitset<3>(funct.to_string().substr(3, 3)), myRF.ReadData1, myRF.ReadData2);
                    myRF.ReadWrite(rs, rt, rd, myALU.ALUresult, 1);
                }
                    break;
                case 0x09:
                case 0x04:
                case 0x23:
                case 0x2B: {
                    //I-type
                    rs = bitset<5>(curIns.to_string().substr(6, 5));
                    rt = bitset<5>(curIns.to_string().substr(11, 5));
                    immediate = bitset<16>(curIns.to_string().substr(16, 16));

                    myRF.ReadWrite(rs, rt, bitset<5>(0x0), bitset<32>(0x0), 0);
                    switch (opcode.to_ulong()) {
                        case 0x09: {
                            //addiu
                            bitset<32> rsvalue = myRF.ReadData1;
                            rsvalue = bitset<32>(rsvalue.to_ulong() + immediate.to_ulong());
                            myRF.ReadWrite(rs, rt, rt, rsvalue, 1);
                        }
                            break;

                        case 0x04: {
                            //beq
                            if (myRF.ReadData1 == myRF.ReadData2) {
                                //immediate=immediate*4
                                immediate = immediate << 2;
                                //signed extension of immediate
                                if (immediate[17] == 1) {
                                    bitset<14> immediate_extend = 0xFFFFFF;
                                    immediate_final = bitset<32>(
                                            immediate_extend.to_string()+immediate.to_string());
                                } else {
                                    bitset<14> immediate_extend = 0x0;
                                    immediate_final = bitset<32>(
                                            immediate_extend.to_string()+immediate.to_string());
                                }
                                PC = bitset<32>(
                                        PC.to_ulong() + immediate_final.to_ulong() + 4);//not sure +4 or +8 !!!
                            }
                        }
                            break;
                        case 0x23: {
                            //lw
                            //signed extension of the offset

                            if (immediate[15] == 1) {
                                bitset<16> immediate_extend = 0xFFFF;
                                immediate_final = bitset<32>(immediate_extend.to_string()+immediate.to_string());
                            } else {
                                bitset<16> immediate_extend = 0x0;
                                immediate_final = bitset<32>(immediate_extend.to_string()+immediate.to_string());
                            }
                            bitset<32> loadAddr = bitset<32>(
                                    myRF.ReadData1.to_ulong() + immediate_final.to_ulong());

                            myDataMem.MemoryAccess(loadAddr, bitset<32>(0), 1, 0);//memory access

                            myRF.ReadWrite(rs, rt, rt, myDataMem.readdata, 1);//write back
                        }
                            break;
                        case 0x2B: {
                            //sw
                            //signed extension of the offset

                            if (immediate[15] == 1) {
                                bitset<16> immediate_extend = 0xFFFF;
                                immediate_final = bitset<32>(immediate_extend.to_string()+immediate.to_string() );
                            } else {
                                bitset<16> immediate_extend = 0x0;
                                immediate_final = bitset<32>(immediate_extend.to_string()+immediate.to_string());
                            }
                            bitset<32> storeAddr = bitset<32>(
                                    myRF.ReadData1.to_ulong() + immediate_final.to_ulong());

                            //cout<<"Store  Address:  "<<storeAddr<<endl;
                            myDataMem.MemoryAccess(storeAddr, myRF.ReadData2, 0, 1);//memory access

                        }
                            break;
                    }
                }
                    break;
                case 0x02: {
                    //J-type
                    //j
                    address = bitset<26>(curIns.to_string().substr(6, 26));
                    PC = bitset<32>(PC.to_ulong() & 0xf0000000 | (address.to_ulong() << 2));
                }
                    break;
            }
            myRF.OutputRF(); // dump RF;
        }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

    
}
