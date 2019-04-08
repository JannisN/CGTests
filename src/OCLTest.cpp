#define __CL_ENABLE_EXCEPTIONS 

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

std::string source2 =
//__attribute__((reqd_work_group_size(X, Y, Z)))
"__kernel void Add(__global const float* a, __global const float* b, uint size, __global float* c)"
"{"
"	int idg0 = get_global_id(0);"
"	int idg1 = get_global_id(1);"
"	int idg2 = get_global_id(2);"
"	int idl0 = get_local_id(0);"
"	int idl1 = get_local_id(1);"
"	int idl2 = get_local_id(2);"
//"	int idy = get_local_id(1);"
//"	int idz = get_local_id(2);"
//"	if (id < size)"
"	{"
"		c[get_global_size(1) * get_global_size(0) * idg2 + get_global_size(0) * idg1 + idg0] = idl0;"
"	}"
"}"
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
			unsigned int maxWorkItemDimension;
			size_t maxWorkGroupSize;
			std::vector<size_t> maxWorkItemSize;
			d.getInfo(CL_DEVICE_NAME, &name);
			d.getInfo(CL_DEVICE_VERSION, &deviceVersion);
			d.getInfo(CL_DRIVER_VERSION, &driverVersion);
			d.getInfo(CL_DEVICE_OPENCL_C_VERSION, &oclCVersion);
			d.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &maxComputeUnits);
			d.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &maxWorkItemDimension);
			d.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
			d.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &maxWorkItemSize);
			std::cout << name << std::endl;
			std::cout << deviceVersion << std::endl;
			std::cout << driverVersion << std::endl;
			std::cout << oclCVersion << std::endl;
			std::cout << maxComputeUnits << std::endl;
			std::cout << maxWorkItemDimension << std::endl;
			std::cout << maxWorkGroupSize << std::endl;
			std::cout << maxWorkItemSize[0] << " " << maxWorkItemSize[1] << " " << maxWorkItemSize[2] << std::endl;
		}
	}

	auto platform = platforms.front();
	auto device = devices.front();

	cl::Program::Sources sources(1, { source.c_str(), source.length() + 1 }); // + 1 nötig?
	cl::Context context(device);
	cl::Program program(context, sources);

	if (program.build("-cl-std=CL1.2"))
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
	//cl::size_t<3> globalWorkSize;
	//kernel.getWorkGroupInfo(device, CL_KERNEL_GLOBAL_WORK_SIZE, &globalWorkSize);
	cl_ulong localMemorySize;
	kernel.getWorkGroupInfo(device, CL_KERNEL_LOCAL_MEM_SIZE, &localMemorySize);

	// program2 -------------------

	cl::Program::Sources sources2(1, { source2.c_str(), source2.length() + 1 }); // + 1 nötig?
	cl::Program program2(context, sources2);

	if (program2.build("-cl-std=CL1.2"))
	{
		std::string log;
		program2.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);
		std::cout << log;
	}

	unsigned int size = 2048;
	std::vector<float> bufferA(2048);
	std::vector<float> bufferB(2048);
	std::vector<float> bufferC(2048);
	cl::Buffer mbA(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, bufferA.size() * sizeof(float));
	cl::Buffer mbB(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, bufferB.size() * sizeof(float));
	cl::Buffer mbC(context, CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY, bufferC.size() * sizeof(float));

	int error2;
	cl::Kernel kernel2(program2, "Add", &error2);

	kernel2.setArg(0, mbA);
	kernel2.setArg(1, mbB);
	kernel2.setArg(2, size);
	kernel2.setArg(3, mbC);

	cl::size_t<3> comppileWorkGroupSize2;
	kernel2.getWorkGroupInfo(device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, &comppileWorkGroupSize2);
	//cl::size_t<3> globalWorkSize2;
	//kernel2.getWorkGroupInfo(device, CL_KERNEL_GLOBAL_WORK_SIZE, &globalWorkSize2);
	cl_ulong localMemorySize2;
	kernel2.getWorkGroupInfo(device, CL_KERNEL_LOCAL_MEM_SIZE, &localMemorySize2);
	cl_ulong privateMemorySize2;
	kernel2.getWorkGroupInfo(device, CL_KERNEL_PRIVATE_MEM_SIZE, &privateMemorySize2);
	size_t workGroupSize2;
	kernel2.getWorkGroupInfo(device, CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize2);
	size_t preferredWorkGroupSizeMultiple;
	kernel2.getWorkGroupInfo(device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, &preferredWorkGroupSizeMultiple);
	// ... work_dim ... global_work_size
	//std::vector<cl::NDRange> globalWorkSizeSet = { 256, 4, 2 };
	//const cl::NDRange localWorkSize[] = { 256, 4, 1 };
	//queue.enqueueNDRangeKernel(kernel2, 3, 0, globalWorkSizeSet[0], localWorkSize);
	cl::Event event;
	//cl::NDRange globalSize(bufferC.size());
	cl::NDRange globalSize(256, 4, 2);
	cl::NDRange localSize(16, 4, 1);
	int error3 = queue.enqueueNDRangeKernel(kernel2, 0, globalSize, localSize);
	// nullptr für auf keine events warten
	error3 = queue.enqueueReadBuffer(mbC, true, 0, bufferC.size() * sizeof(float), bufferC.data());
	//std::cout << std::string(bufferC.data(), bufferC.size());

	return 0;
}