#pragma once
#include "Pattern_detector.h"

void Pattern_detector::ClockGeneration()
{
	while (true)
	{
		clk.write(SC_LOGIC_1);
		wait(10, SC_NS);
		clk.write(SC_LOGIC_0);
		wait(10, SC_NS);
	}
}

void Pattern_detector::Reseting()
{
	while (true)
	{
		cout << "\nRESET issued...\n\n";
		start.write(SC_LOGIC_0);
		rst.write(SC_LOGIC_1);
		wait(15, SC_NS);
		rst.write(SC_LOGIC_0);
		start.write(SC_LOGIC_1);
		wait(30, SC_NS);
		start.write(SC_LOGIC_0);
		wait();
	}
}

void Pattern_detector::Signal_generation()
{
	result.write("000");
	while (true) {
		conv_start.write(SC_LOGIC_0);
		done.write(SC_LOGIC_0);

		 if (clk.event() && clk.read() == SC_LOGIC_1) {
			if (start.read() == SC_LOGIC_1) {
				while (start.read() != SC_LOGIC_0) {
					wait();
				}
				conv_start.write(SC_LOGIC_1);
				wait();
				conv_start.write(SC_LOGIC_0);
				while (true) {
					wait();
					if (conv_done1.read() == SC_LOGIC_1 && conv_done2.read() == SC_LOGIC_1 && conv_done3.read() == SC_LOGIC_1) {
						done.write(SC_LOGIC_1);
						break;
					}
				}

				std::vector<int> max_results = { maxOut1.read().to_int(), maxOut2.read().to_int(), maxOut3.read().to_int() };
				int temp = *(max_element(max_results.begin(), max_results.end()));
				wait();

				if (temp == maxOut1.read().to_int()) {
					result.write("001");
				}
				else if (temp == maxOut2.read().to_int()) {
					result.write("010");
				}
				else {
					result.write("100");
				}

			}
		}
		wait();
	}
}
void Pattern_detector::monitor_signals() {
	while (true) {
		sc_report_handler::set_actions(SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_,
			SC_DO_NOTHING);
		wait(20, SC_NS);
		if (done.read() == 1){
			wait(20, SC_NS);
			cout << "\nFinal Result is :  "<<result.read()<<endl<<endl;

		}
	}
}
int sc_main(int argc, char *argv[])
{
	Pattern_detector* Pattern_detector_TB = new Pattern_detector("Pattern_detector_TB");

	cout << "starting...\n";
	sc_start(1000, SC_NS);
	return 0;
}