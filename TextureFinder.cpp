
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <iostream>
#include <fstream>
#include <CL/cl.hpp>
#include <array>


int main()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	_ASSERT(platforms.size() > 0);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;

	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	_ASSERT(devices.size() > 0);

	auto device = devices.front();

	const int version = 3; //2 == 1.12 and before, 3 == 1.13 and 1.14

	std::string fileName;

	if (version == 2)
		fileName = "kernelFile2.cl";
	else
		fileName = "kernelFile3.cl";

	std::ifstream kernelFile(fileName);
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));


	
	cl::Context context(device);
	cl::Program program(context, sources);

	auto err = program.build("-cl-std=CL1.2");

	std::cout << std::to_string(err)<<std::endl;//print error code


	int facecount = 32;
	const int arraysize = 160;//5 times facecount
	std::array<int, arraysize> formation =
		{{0, 0, 0, 0, 0,
			1, 0, 0, 0, 0,
			2, 0, 0, 0, 3,
			3, 0, 0, 0, 0,
			4, 0, 0, 0, 2,
			5, 0, 0, 0, 0,
			6, 0, 0, 0, 3,
			7, 0, 0, 0, 2,

			0, 0, 1, 0, 1,
			1, 0, 1, 0, 0,
			2, 0, 1, 0, 3,
			3, 0, 1, 0, 1,
			4, 0, 1, 0, 0,
			5, 0, 1, 0, 0,
			6, 0, 1, 0, 2,
			7, 0, 1, 0, 0,

			0, 0, 2, 0, 1,
			1, 0, 2, 0, 1,
			2, 0, 2, 0, 3,
			3, 0, 2, 0, 3,
			4, 0, 2, 0, 0,
			5, 0, 2, 0, 0,
			6, 0, 2, 0, 1,
			7, 0, 2, 0, 2,

			0, 0, 3, 0, 0,
			1, 0, 3, 0, 3,
			2, 0, 3, 0, 2,
			3, 0, 3, 0, 1,
			4, 0, 3, 0, 1,
			5, 0, 3, 0, 1,
			6, 0, 3, 0, 0,
			7, 0, 3, 0, 1 }} ;//x, y, z, faceid, rotation


	//0=top, 1=bottom, 2=west, 3=east, 4=south, 5=north

	

	std::array<int, 3> loc =  {0, 0, 0} ;
	int xlength = 100000;
	int zlength = 100000;
	int yrangesize = 1;
	int ymin = 56;

	cl::Buffer facesbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * arraysize, formation.data());
	cl::Buffer facecountbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &facecount);
	cl::Buffer resultbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * 3, loc.data());
	cl::Buffer xlengthbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &xlength);
	cl::Buffer zlengthbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &zlength);
	cl::Buffer yminbuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &ymin);


	cl::Kernel kernel(program, "kernelFile");
	kernel.setArg(0, facesbuf);
	kernel.setArg(1, facecountbuf);
	kernel.setArg(2, resultbuf);
	kernel.setArg(3, xlengthbuf);
	kernel.setArg(4, zlengthbuf);
	kernel.setArg(5, yminbuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NDRange(NULL), cl::NDRange(xlength, yrangesize, zlength));

	queue.enqueueReadBuffer(resultbuf, CL_TRUE, 0, sizeof(int) * 3, loc.data());

	queue.finish();

	std::cout << "X: " << loc[0] << " Y: " << loc[1] << " Z: " << loc[2];


}

