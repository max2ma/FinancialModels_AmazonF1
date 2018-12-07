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
 * S0:		 stock price at time 0
 * K:		 strike price
 * rate:		 interest rate
 * volatility:	 volatility of stock
 * T:		 time period of the option
 *
 *
 * callR:	-c reference value for call price
 * putR:		-p reference value for put price
 * binary_name:  -a the .xclbin binary name
 * kernel_name:  -n the kernel name
 * num_sims:					-s number of simulations
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
using namespace std;

namespace Params
{
	float theta = 0.019;
	float kappa = 6.21;
	float xi = 0.61;
	float rho = -0.7;
	float initprice = 100;
	float strikeprice = 100;
	float rate = 0.0319;
	float volatility = 0.10201;
	float time = 1.0;
	const char *kernel_name=KERNEL;     // -n
	const char *binary_name=KERNEL ".hw." PLATFORM ".awsxclbin";     // -a
}
void usage(char* name)
{
	cout<<"Usage: "<<name <<" [-b binary_file_name]" <<" [-c call_price]" <<" [-p put_price]" <<endl;
}
int main(int argc, char** argv)
{
	int opt,NUM_CU=16, sims = 1024, steps=1024;
	float callR=-1, putR=-1;
	bool flagc=false,flagp=false;
	while((opt=getopt(argc,argv,"n:s:k:b:c:p:"))!=-1){
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
			case 'n':
				NUM_CU=atoi(optarg);
				break;
			case 's':
				sims=atoi(optarg);
				break;
			case 'k':
				steps=atoi(optarg);
				break;
			default:
				usage(argv[0]);
				return -1;
		}
	}
	fstream is(KERNEL ".parameters", ios::in);
	if(!is){
		cerr << "Cannot open parameter file: " KERNEL ".parameters" <<endl;
		return -1;
	}
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
		} else if (line.substr(0, strlen("theta:")) == "theta:") {
			Params::theta = stod(line.substr(strlen("theta:")+1));
		} else if (line.substr(0, strlen("kappa:")) == "kappa:") {
			Params::kappa = stod(line.substr(strlen("kappa:")+1));
		} else if (line.substr(0, strlen("xi:")) == "xi:") {
			Params::xi = stod(line.substr(strlen("xi:")+1));
		} else if (line.substr(0, strlen("rho:")) == "rho:") {
			Params::rho = stod(line.substr(strlen("rho:")+1));
		} else if (line.substr(0, strlen("kernel_name:")) == "kernel_name:") {
			//Params::kernel_name = line.substr(strlen("kernel_name:")+1).c_str();
		} else {
			cerr << "Unknown parameter: " << line << endl;
		}
	}

#ifdef PURE_C
	float h_call[1]={0}, h_put[1]={0};
#else
	try {
		ifstream ifstr(Params::binary_name);
		if(!ifstr){
			cerr << "Cannot open binary file: " << Params::binary_name<<endl;
			return -1;
		}

		const string programString(istreambuf_iterator<char>(ifstr),
				(istreambuf_iterator<char>()));
		vector<float, aligned_allocator<float> > h_call(NUM_CU),h_put(NUM_CU);
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		cl::Context context(CL_DEVICE_TYPE_ACCELERATOR);
		vector<cl::Device> devices=context.getInfo<CL_CONTEXT_DEVICES>();

		cl::Program::Binaries binaries(1, make_pair(programString.c_str(), programString.length()));
		cl::Program program(context,devices,binaries);
		try {
			program.build(devices);
		} catch (cl::Error err) {
			if (err.err() == CL_BUILD_PROGRAM_FAILURE) {
				string info;
				program.getBuildInfo(devices[0],CL_PROGRAM_BUILD_LOG, &info);
				cout << info << endl;
				return EXIT_FAILURE;
			}
			else throw err;
		}

		cl::CommandQueue commandQueue(context, devices[0], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE);

		cl::Kernel kernel(program,Params::kernel_name);

		cl::Buffer d_call(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
				sizeof(float)*NUM_CU, h_call.data());
		cl::Buffer d_put(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
				sizeof(float)*NUM_CU, h_put.data());
		std::vector<cl::Memory> outBufVec;
		outBufVec.push_back(d_call);
		outBufVec.push_back(d_put);
#endif

		clock_t start = clock();
		cout << "Starting execution. Time=" << Params::time
			<< " rate=" << Params::rate
			<< " volatility =" << Params::volatility
			<< " initprice=" << Params::initprice
			<< " strikeprice=" << Params::strikeprice
			<< " theta=" << Params::theta
			<< " kappa=" << Params::kappa
			<< " xi=" << Params::xi
			<< " rho=" << Params::rho
			<< " No.simulations="<<sims
			<< " No.partitions="<<steps
			<< endl;
		auto kernelFunctor = cl::KernelFunctor<cl::Buffer,cl::Buffer,float,float,float,float,float,float,float,float,float,int, int, int>(kernel);

		cl::EnqueueArgs enqueueArgs(commandQueue,cl::NDRange(1),cl::NDRange(1));
		cl::Event event[NUM_CU];
		for(int i=0;i<NUM_CU;i++)
			event[i] = kernelFunctor(enqueueArgs,
					d_call,d_put,
					Params::theta,
					Params::kappa,
					Params::xi,
					Params::rho,
					Params::time,
					Params::rate,
					Params::volatility,
					Params::initprice,
					Params::strikeprice,
					sims,
					steps,
					i
					);
		/*
		int i_args=0;
		string info;
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, d_call);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, d_put);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::theta);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::kappa);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::xi);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::rho);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::time);
		
		kernel.getArgInfo(i_args, CL_KERNEL_ARG_NAME, &info);
		cout << "info of arg["<<i_args<<"] is " << info <<endl;
		kernel.setArg(i_args++, Params::rate);
		
		kernel.setArg(i_args++, Params::volatility);
		kernel.setArg(i_args++, Params::initprice);
		kernel.setArg(i_args++, Params::strikeprice);
		kernel.setArg(i_args++, sims);
		kernel.setArg(i_args++, steps);
		for(int i=0;i<NUM_CU;i++){
			kernel.setArg(i_args, i);
			commandQueue.enqueueTask(kernel);
		}
*/
		commandQueue.finish();
		commandQueue.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
		commandQueue.finish();

		clock_t t = clock() - start;
		cout << "Execution completed"<<endl;
		cout << "Execution time "<< (float)t /CLOCKS_PER_SEC <<" s"<<endl;

		float pCall=0, pPut=0;
		for(int i=0;i<NUM_CU;i++){
			pCall+= h_call[i];
			pPut += h_put[i];
		}
		pCall/=NUM_CU*expf(Params::rate*Params::time);
		pPut/=NUM_CU*expf(Params::rate*Params::time);
		cout<<"the call price is: "<<pCall<<'\t';
		cout<<endl;
		cout<<"the put price is: "<<pPut<<'\t';
		cout<<endl;
#ifndef PURE_C
	} catch (cl::Error err) {
		cerr << "Error:\t" << err.what() << "Code:\t" << err.err() << endl; 
		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}
