#include <iostream>

#include "Utils.h"

#include "CL/cl.hpp"

std::string source =
"__kernel void HelloWorld(__global char* data)\n"
"{											  \n"
"	data[0] = 'H';							  \n"
"	data[1] = 'E';							  \n"
"	data[2] = 'L';							  \n"
"	data[3] = 'L';							  \n"
"	data[4] = 'O';							  \n"
"	data[5] = ' ';							  \n"
"	data[6] = 'W';							  \n"
"	data[7] = 'O';							  \n"
"	data[8] = 'R';							  \n"
"	data[9] = 'L';							  \n"
"	data[10] = 'D';							  \n"
"	data[11] = '!';							  \n"
"	data[12] = '\\n';						  \n"
"}											  "
;

int main()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	
	std::vector<cl::Device> devices;

	for (auto p : platforms)
	{
		std::string name;
		std::string vendor;
		std::string version;
		std::string profile;
		std::string extensions;
		p.getInfo(CL_PLATFORM_NAME, &name);
		p.getInfo(CL_PLATFORM_VENDOR, &vendor);
		p.getInfo(CL_PLATFORM_VERSION, &version);
		p.getInfo(CL_PLATFORM_PROFILE, &profile);
		p.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
		std::cout << name << std::endl;
		std::cout << vendor << std::endl;
		std::cout << version << std::endl;
		std::cout << profile << std::endl;
		auto extensionList = cg::split(extensions, ' ');
		for (auto s : extensionList)
			std::cout << s << std::endl;

		//std::vector<cl::Device> devices;
		p.getDevices(CL_DEVICE_TYPE_GPU, &devices);

		for (auto d : devices)
		{
			std::string name;
			std::string deviceVersion;
			std::string driverVersion;
			std::string oclCVersion;
			int maxComputeUnits;
			d.getInfo(CL_DEVICE_NAME, &name);
			d.getInfo(CL_DEVICE_VERSION, &deviceVersion);
			d.getInfo(CL_DRIVER_VERSION, &driverVersion);
			d.getInfo(CL_DEVICE_OPENCL_C_VERSION, &oclCVersion);
			d.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &maxComputeUnits);
			std::cout << name << std::endl;
			std::cout << deviceVersion << std::endl;
			std::cout << driverVersion << std::endl;
			std::cout << oclCVersion << std::endl;
			std::cout << maxComputeUnits << std::endl;
		}
	}

	auto platform = platforms.front();
	auto device = devices.front();

	cl::Program::Sources sources(1, { source.c_str(), source.length() + 1 }); // + 1 nötig?
	cl::Context context(device);
	cl::Program program(context, sources);

	if (!program.build("-cl-std=CL1.2"))
	{
		std::string log;
		program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);
		std::cout << log;
	}

	std::vector<char> buffer(13);
	cl::Buffer memoryBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, buffer.size());

	int error;
	cl::Kernel kernel(program, "HelloWorld", &error);
	kernel.setArg(0, memoryBuffer);

	cl::CommandQueue queue(context, device);
	//queue.enqueueNDRangeKernel(kernel, 0, )
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memoryBuffer, true, 0, buffer.size(), buffer.data());

	std::cout << std::string(buffer.data(), buffer.size());

	cl::size_t<3> workGroupSize;
	kernel.getWorkGroupInfo(device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &workGroupSize);
	cl::size_t<3> globalWorkSize;
	kernel.getWorkGroupInfo(device, CL_KERNEL_GLOBAL_WORK_SIZE, &globalWorkSize);
	unsigned long localMemorySize;
	kernel.getWorkGroupInfo(device, CL_KERNEL_LOCAL_MEM_SIZE, &localMemorySize);

	return 0;
}