#include <iostream>
#include <systemc.h>

#define Img_ram "mem.txt"

template<int addressBits, int wordSize>
class RegisterFile : public sc_module
{
public:
	sc_in<sc_logic> clk, rst, read1, read2, read3, write;
	sc_in<sc_lv<addressBits> > Addr1, Addr2, Addr3;
	sc_in<sc_lv<wordSize> > Din;
	sc_out<sc_lv<wordSize> > Dout1, Dout2, Dout3;

	int rows;
	sc_uint <wordSize> *mem;

	void meminit();
	void memread();
	void memwrite();
	void resetting();

	SC_HAS_PROCESS(RegisterFile);
	RegisterFile(sc_module_name);
};

template<int addressBits, int wordSize>
RegisterFile<addressBits, wordSize>::RegisterFile(sc_module_name)
{
	rows = int(pow(2.0, addressBits));
	mem = new sc_uint<wordSize>[rows];

	SC_THREAD(meminit);

	SC_METHOD(memread);
	sensitive << Addr1 << Addr2 << Addr3 << read1, read2, read3;

	SC_THREAD(memwrite);
	sensitive << clk;
};

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::meminit()
{
	ifstream in(Img_ram);
	int i;
	sc_lv<wordSize> data;

	for (i = 0; i < rows; i++) {
		in >> data;
		mem[i] = data;
	}

	cout << "\n*** MEMORY INIT DONE ***\n";
};

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::memwrite()
{
	sc_uint<addressBits> ad;
	while (true)
	{
		if (clk.event() && clk == '1') {
			if (write == '1') {
				ad = Addr1;
				mem[ad] = Din;
				cout << "writing @" << ad << ": " << Din.read() << "( " << mem[ad] << " )" << endl;
			}
		}
		wait();
	}
};

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::resetting()
{
	sc_uint<addressBits> ad;
	while (true)
	{
		if (rst == '1') {
			cout << "*** RESETTING MEMORY ***\n";
			for (int i = 0; i < rows; i++)
				mem[i] = 0;
		}
		wait();
	}
};

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::memread()
{
	sc_uint<addressBits> ad;
	if (read1 == '1') {
		ad = Addr1;
		Dout1 = mem[ad];
	}

	if (read2 == '1') {
		ad = Addr2;
		Dout2 = mem[ad];
	}

	if (read3 == '1') {
		ad = Addr3;
		Dout3 = mem[ad];
	}
};