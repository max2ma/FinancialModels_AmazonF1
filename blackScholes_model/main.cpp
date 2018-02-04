/* 
======================================================
 Copyright 2016 Liang Ma

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
======================================================
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Host code defining all the parameters and launching the kernel. 
*
* Exception handling is enabled (CL_HPP_ENABLE_EXCEPTIONS) to make host code simpler.
*
* The global and local size are set to 1 since the kernel is written in C/C++ instead of OpenCL.
*
* Several input parameters for the simulation are defined in namespace Params
* and can be changed by using command line options. Only the kernel name must
* be defined.
*
* S0:		-s stock price at time 0
* K:		-k strike price
* rate:		-r interest rate
* volatility:	-v volatility of stock
* T:		-t time period of the option
*
*
* callR:	-c reference value for call price
* putR:		-p reference value for put price
* binary_name:  -a the .xclbin binary name
* kernel_name:  -n the kernel name
*----------------------------------------------------------------------------
*/

#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include "xcl2.hpp"

#ifdef USE_PROTOBUF
#include "bs.pb.h"
#include <google/protobuf/text_format.h>
#endif
using namespace std;

namespace Params 
{
	double initprice = 100;		    // -s
	double strikeprice = 110;			    // -k
	double rate = 0.05;   		// -r
	double volatility = 0.2;	// -v
	double time = 1.0;			    // -t
	const char *kernel_name=KERNEL;     // -n
	const char *binary_name=KERNEL ".hw.xilinx_xil-accel-rd-ku115_4ddr-xpr.awsxclbin";     // -a
}
void usage(char* name)
{
    cerr<<"Usage: "<<name
        <<" [-b binary_file_name]"
        <<" [-c call_price]"
        <<" [-p put_price]"
        <<endl;
}
int main(int argc, char** argv)
{
	int opt;
	double callR=-1, putR=-1;
	bool flagc=false,flagp=false;
	while((opt=getopt(argc,argv,"b:c:p:"))!=-1){
		switch(opt){
			case 'b':
				Params::binary_name=optarg;
				break;
			case 'c':
				callR=atof(optarg);
				flagc=true;
				break;
			case 'p':
				putR=atof(optarg);
				flagp=true;
				break;
			default:
				usage(argv[0]);
				return -1;
		}
	}

#ifdef USE_PROTOBUF
	parameters::blackScholes params;
#endif
	fstream is(KERNEL ".parameters", ios::in);
	if(!is){
		cerr << "Cannot open parameter file: " KERNEL ".parameters" <<endl;
		return -1;
	}
#ifdef USE_PROTOBUF
	const string str(istreambuf_iterator<char>(is),
		(istreambuf_iterator<char>()));
	google::protobuf::TextFormat::ParseFromString(str, &params);

	Params::time = params.time();
	Params::rate == params.rate();
	Params::volatility = params.volatility();
	Params::initprice = params.initprice();
	Params::strikeprice == params.strikeprice();
#else
	string line;
	while (getline (is, line)) {
	    if (line.substr(0, strlen("initprice:")) == "initprice:") {
		Params::initprice = stod(line.substr(strlen("initprice:")+1));
	    } else if (line.substr(0, strlen("strikeprice:")) == "strikeprice:") {
		Params::strikeprice = stod(line.substr(strlen("strikeprice:")+1));
	    } else if (line.substr(0, strlen("rate:")) == "rate:") {
		Params::rate = stod(line.substr(strlen("rate:")+1));
	    } else if (line.substr(0, strlen("volatility:")) == "volatility:") {
		Params::volatility = stod(line.substr(strlen("volatility:")+1));
	    } else if (line.substr(0, strlen("time:")) == "time:") {
		Params::time = stod(line.substr(strlen("time:")+1));
	    } else if (line.substr(0, strlen("kernel_name:")) == "kernel_name:") {
		//Params::kernel_name = line.substr(strlen("kernel_name:")+1).c_str();
	    } else {
		cerr << "Unknown parameter: " << line << endl;
	    }
	}
#endif
	ifstream ifstr(Params::binary_name); 
	if(!ifstr){
		cerr << "Cannot open binary file: " << Params::binary_name<<endl;
		return -1;
	}
	const string programString(istreambuf_iterator<char>(ifstr),
		(istreambuf_iterator<char>()));
	vector<float,aligned_allocator<float>> h_call(1),h_put(1);

	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		cl::Context context(CL_DEVICE_TYPE_ACCELERATOR);
		vector<cl::Device> devices=context.getInfo<CL_CONTEXT_DEVICES>();

		cl::Program::Binaries binaries(1, make_pair(programString.c_str(), programString.length()));
		cl::Program program(context,devices,binaries);
		try
		{
			program.build(devices);
		}
		catch (cl::Error err)
		{
			if (err.err() == CL_BUILD_PROGRAM_FAILURE)
			{
				string info;
				program.getBuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &info);
				cerr << info << endl;
				return EXIT_FAILURE;
			}
			else throw err;
		}

		cl::CommandQueue commandQueue(context, devices[0]);
		
    cl::Kernel kernel(program,Params::kernel_name);
    auto kernelFunctor = cl::KernelFunctor<cl::Buffer,cl::Buffer,float,float,float,float,float>(kernel);

    cout << "Starting execution. Time=" << Params::time
	<< " rate=" << Params::rate 
	<< " volatility =" << Params::volatility
	<< " initprice=" << Params::initprice 
	<< " strikeprice=" << Params::strikeprice 
	<< endl;
    cl::Buffer d_call(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            sizeof(float), h_call.data());
    cl::Buffer d_put(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            sizeof(float), h_put.data());
    std::vector<cl::Memory> outBufVec;
    outBufVec.push_back(d_call);
    outBufVec.push_back(d_put);
		//cl::Buffer d_call = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float));
		//cl::Buffer d_put  = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float));

		clock_t start = clock();
		cl::EnqueueArgs enqueueArgs(commandQueue,cl::NDRange(1),cl::NDRange(1));
		cl::Event event = kernelFunctor(enqueueArgs,
						d_call,d_put,
						Params::time,
						Params::rate,
						Params::volatility,
						Params::initprice,
						Params::strikeprice
						);

    commandQueue.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
		commandQueue.finish();
		event.wait();

		clock_t t = clock() - start;
		cl_ulong time_start, time_stop;
		event.getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
		event.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_stop);
		double Seconds = (double)(time_stop  - time_start)/1e9;
		cout << "Execution completed"<<endl;
		cout << "Execution time "<< (float)t /CLOCKS_PER_SEC <<" s"<<endl;
		//cout << "Execution time "<< Seconds <<" s"<<endl;
		
		//cl::copy(commandQueue, d_call, h_call.begin(), h_call.end());
		//cl::copy(commandQueue, d_put, h_put.begin(), h_put.end());
		cout<<"the call price is: "<<h_call[0]<<'\t';
		if(flagc)
			cout<<"the difference with the reference value is "<<fabs(h_call[0]/callR-1)*100<<'%'<<endl;
		cout<<endl;
		cout<<"the put price is: "<<h_put[0]<<'\t';
		if(flagp)
			cout<<"the difference with the reference value is "<<fabs(h_put[0]/putR-1)*100<<'%'<<endl;
		cout<<endl;
	}
	catch (cl::Error err)
	{
		cerr
			<< "Error:\t"
			<< err.what()
			<< "\nCode:\t"
			<< err.err()
			<< endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
