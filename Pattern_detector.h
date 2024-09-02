#pragma once
#include <systemc.h>
#include "Convolution.h"
#include "Img_RAM.h"

const int DATA_WIDTH = 8;
const int KERNEL_WIDTH = 3;
const int IMAGE_WIDTH = 4;
const int STRIDE = 1;

const int el11 = 0;
const int el21 = 1;
const int el31 = 0;
const int el41 = 1;
const int el51 = 1;
const int el61 = 1;
const int el71 = 0;
const int el81 = 1;
const int el91 = 0;
const int BIAS1 = -1;

const int el12 = 1;
const int el22 = 1;
const int el32 = 1;
const int el42 = 1;
const int el52 = 0;
const int el62 = 0;
const int el72 = 1;
const int el82 = 1;
const int el92 = 1;
const int BIAS2 = -2;

const int el13 = 1;
const int el23 = 1;
const int el33 = 1;
const int el43 = 0;
const int el53 = 1;
const int el63 = 0;
const int el73 = 0;
const int el83 = 1;
const int el93 = 0;
const int BIAS3 = -2;

SC_MODULE(Pattern_detector)
{
	sc_signal<sc_logic> clk, rst, start;
	sc_signal<sc_logic> done;
	sc_signal<sc_lv<3>> result;

	sc_signal<sc_logic> conv_start, conv_done1, conv_done2, conv_done3;
	sc_signal<sc_logic> read1, read2, read3, write;
	sc_signal<sc_lv<8>> memData1, memData2, memData3, maxOut1, maxOut2, maxOut3, inp;
	sc_signal<sc_lv<4>> addr1, addr2, addr3;

	Convolution<el11, el21, el31, el41, el51, el61, el71, el81, el91, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS1>* conv1;
	Convolution<el12, el22, el32, el42, el52, el62, el72, el82, el92, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS2>* conv2;
	Convolution<el13, el23, el33, el43, el53, el63, el73, el83, el93, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS3>* conv3;
	RegisterFile<4, 8> *Ram1;

	SC_CTOR(Pattern_detector)  {
		conv1 = new Convolution<el11, el21, el31, el41, el51, el61, el71, el81, el91, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS1>("Convolution1");
		conv1->clk(clk);
		conv1->rst(rst);
		conv1->conv_start(conv_start);
		conv1->conv_done(conv_done1);
		conv1->memReadData(memData1);
		conv1->memRead(read1);
		conv1->memAddr(addr1);
		conv1->maxPoolOut(maxOut1);

		conv2 = new Convolution<el12, el22, el32, el42, el52, el62, el72, el82, el92, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS2>("Convolution2");
		conv2->clk(clk);
		conv2->rst(rst);
		conv2->conv_start(conv_start);
		conv2->conv_done(conv_done2);
		conv2->memReadData(memData2);
		conv2->memRead(read2);
		conv2->memAddr(addr2);
		conv2->maxPoolOut(maxOut2);

		conv3 = new Convolution<el13, el23, el33, el43, el53, el63, el73, el83, el93, DATA_WIDTH, KERNEL_WIDTH, IMAGE_WIDTH, STRIDE, BIAS3>("Convolution3");
		conv3->clk(clk);
		conv3->rst(rst);
		conv3->conv_start(conv_start);
		conv3->conv_done(conv_done3);
		conv3->memReadData(memData3);
		conv3->memRead(read3);
		conv3->memAddr(addr3);
		conv3->maxPoolOut(maxOut3);

		Ram1 = new RegisterFile<4, 8>("Memory");
		Ram1->clk(clk);
		Ram1->rst(rst);
		Ram1->Addr1(addr1);
		Ram1->Addr2(addr2);
		Ram1->Addr3(addr3);
		Ram1->Din(inp);
		Ram1->Dout1(memData1);
		Ram1->Dout2(memData2);
		Ram1->Dout3(memData3);
		Ram1->write(write);
		Ram1->read1(read1);
		Ram1->read2(read2);
		Ram1->read3(read3);

		SC_THREAD(ClockGeneration);
		SC_THREAD(Reseting);
		SC_THREAD(Signal_generation);
		sensitive << clk.posedge_event() << rst;
		SC_THREAD(monitor_signals);
		sensitive << clk.posedge_event();

		sc_trace_file* VCDFile;
		VCDFile = sc_create_vcd_trace_file("Waveforms_6");
		VCDFile->set_time_unit(1, SC_NS);
		sc_trace(VCDFile, clk, "clk");
		sc_trace(VCDFile, rst, "rst");
		sc_trace(VCDFile, start, "start");
		sc_trace(VCDFile, done, "done");
		sc_trace(VCDFile, result, "result");
	}

	void ClockGeneration();
	void Reseting();
	void Signal_generation();
	void monitor_signals();
};
