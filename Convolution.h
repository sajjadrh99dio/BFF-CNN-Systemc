#pragma once
#include <systemc.h>
#include <vector>
#include <iostream>
#include <algorithm>
#define num_of_clocks 20
template <
	int el1, int el2, int el3,
	int el4, int el5, int el6,
	int el7, int el8, int el9,
	int DATA_WIDTH, int KERNEL_WIDTH, int IMAGE_WIDTH, int STRIDE, int BIAS
>
SC_MODULE(Convolution) {
	sc_in<sc_logic> clk, rst, conv_start;
	sc_in<sc_lv<8>> memReadData;
	sc_out<sc_lv<4>> memAddr;
	sc_out<sc_lv<8>> maxPoolOut;
	sc_out<sc_logic> conv_done, memRead;

	SC_CTOR(Convolution) {
		SC_THREAD(Conv_process);
		sensitive << clk.pos() << rst;
	}

	void Conv_process() {
		const int kernel[KERNEL_WIDTH][KERNEL_WIDTH] = { { el1, el2, el3 }, { el4, el5, el6 }, { el7, el8, el9 } };
		std::vector<int> addresses = {
			0, 1, 2, 4, 5, 6, 8, 9, 10,
			1, 2, 3, 5, 6, 7, 9, 10, 11,
			4, 5, 6, 8, 9, 10, 12, 13, 14,
			5, 6, 7, 9, 10, 11, 13, 14, 15
		};

		while (true) {
			conv_done = SC_LOGIC_0;

			if (rst == SC_LOGIC_1) {
				maxPoolOut = "00000000";
				conv_done = SC_LOGIC_0;
				//wait();
			}
			else {
				if (conv_start == SC_LOGIC_1) {
					//cout << "Starting the Convolution Process: " << endl;
					int vals[4] = { 0, 0, 0, 0 };
					//wait();

					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 9; j++) {
							memAddr.write(addresses[i * 9 + j]);
							memRead.write(SC_LOGIC_1);
							wait(2, SC_NS);
							memRead.write(SC_LOGIC_0);
							//cout << "\ndata=" << memReadData.read().to_int();
							//cout << "\nKernel=" << kernel[j / 3][j % 3];
							vals[i] += memReadData.read().to_int() * kernel[j / 3][j % 3];
							//wait();
						}
						//cout << "\n Bias:" << BIAS;
							vals[i] += BIAS;
						//wait();
					}
					//for (int i = 0; i < num_of_clocks - 1; i++)     //Synchronization With the RTL model
						//wait(clk.posedge_event());


					cout << endl << "Convolution Results : [ " << vals[0] << ", " << vals[1] << ", " << vals[2] << ", " << vals[3] << " ]" << endl;

					// ReLU part
					for (int i = 0; i < 4; i++) {
						vals[i] = std::max(vals[i], 0);
					}

					cout << endl << "Relu Results : " << ": [ " << vals[0] << ", " << vals[1] << ", " << vals[2] << ", " << vals[3] << " ]" << endl;

					// MaxPool part
					int maxVal = *std::max_element(vals, vals + 4);
					//wait();

					cout << endl << "MaxPool Results result : " << maxVal << endl;

					maxPoolOut.write(maxVal);
					conv_done.write(SC_LOGIC_1);
				}
			}
			wait();
		}
	}
};
