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
* ML_cl.h is the OpenCL library file <CL/cl.hpp>. Currently the version shipped with SDAccel is buggy.
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
* K:		  strike price
* rate:	interest rate
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

// This should be used when cl.hpp from SDAccel works.
//#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include "xcl2.hpp"
using namespace std;

namespace Params
{
	double theta = 0.04;
	double kappa = 0.5;
	double xi = 1;
	double rho = 0;
	double S0 = 100;
	double K = 100;
	double rate = 0.0;
	double volatility = 0.04;
	double T = 1.0;
	double upB=110;
	double lowB=90;
	char *kernel_name=NULL;     // -n
	char *binary_name=NULL;     // -a
}
void usage(char* name)
{
    cout<<"Usage: "<<name
        <<" -a opencl_binary_name"
        <<" -n kernel_name"
        <<" [-c call_price]"
        <<" [-p put_price]"
        <<endl;
}
int main(int argc, char** argv)
{
	int opt;
	double callR=-1, putR=-1;
	bool flaga=false,flagc=false,flagp=false,flagn=false;
	while((opt=getopt(argc,argv,"n:a:c:p:"))!=-1){
		switch(opt){
			case 'n':
				Params::kernel_name=optarg;
				flagn=true;
				break;
			case 'a':
				Params::binary_name=optarg;
				flaga=true;
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
	// Check the mandatory argument.
	if(!flagn) {
		usage(argv[0]);
		return -1;
	}
	ifstream ifstr(Params::binary_name);
	const string programString(istreambuf_iterator<char>(ifstr),
		(istreambuf_iterator<char>()));
	vector<float, aligned_allocator<float> > h_call(1),h_put(1);

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
				cout << info << endl;
				return EXIT_FAILURE;
			}
			else throw err;
		}

		cl::CommandQueue commandQueue(context, devices[0]);

//		typedef cl::make_kernel<cl::Buffer,cl::Buffer,float,float,float,float,float,float,float,float,float,float,float> kernelType;
//		kernelType kernelFunctor = kernelType(program, Params::kernel_name);
    
		cl::Kernel kernel(program,Params::kernel_name);
    auto kernelFunctor = cl::KernelFunctor<cl::Buffer,cl::Buffer,float,float,float,float,float,float,float,float,float,float,float>(kernel);


		cl::Buffer d_call(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
				              sizeof(float), h_call.data());
		cl::Buffer d_put(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
					            sizeof(float), h_put.data());
		std::vector<cl::Memory> outBufVec;
	  outBufVec.push_back(d_call);
		outBufVec.push_back(d_put);
		clock_t start = clock();
		cl::EnqueueArgs enqueueArgs(commandQueue,cl::NDRange(1),cl::NDRange(1));
		cl::Event event = kernelFunctor(enqueueArgs,
						d_call,d_put,
						Params::theta,
						Params::kappa,
						Params::xi,
				 		Params::rho,
						Params::T,
				 		Params::rate,
						Params::volatility,
						Params::S0,
						Params::K,
						Params::upB,
						Params::lowB
						);

		commandQueue.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
		commandQueue.finish();
		event.wait();

		//cl::copy(commandQueue, d_call, h_call.begin(), h_call.end());
		//cl::copy(commandQueue, d_put, h_put.begin(), h_put.end());
		clock_t t = clock() - start;
		cout << "The execution lasts for "<< (float)t /CLOCKS_PER_SEC <<" s (CPU time)."<<endl;
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
			<< endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
