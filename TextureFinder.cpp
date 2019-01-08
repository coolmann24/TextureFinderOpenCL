
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <iostream>
#include <fstream>
#include <CL/cl.hpp>
#include <array>

int *rotate90deg(int *formation, int facecount);


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
		{{0, 0, 0, 4, 3,
			1, 0, 0, 4, 3,
			2, 0, 0, 4, 3,
			3, 0, 0, 4, 3,
			4, 0, 0, 4, 3,
			5, 0, 0, 4, 3,
			6, 0, 0, 4, 3,
			7, 0, 0, 4, 1,

			0, 1, 0, 4, 2,
			1, 1, 0, 4, 1,
			2, 1, 0, 4, 0,
			3, 1, 0, 4, 3,
			4, 1, 0, 4, 0,
			5, 1, 0, 4, 1,
			6, 1, 0, 4, 3,
			7, 1, 0, 4, 1,

			0, 2, 0, 4, 1,
			1, 2, 0, 4, 2,
			2, 2, 0, 4, 3,
			3, 2, 0, 4, 3,
			4, 2, 0, 4, 2,
			5, 2, 0, 4, 1,
			6, 2, 0, 4, 3,
			7, 2, 0, 4, 3,

			0, 3, 0, 4, 3,
			1, 3, 0, 4, 3,
			2, 3, 0, 4, 1,
			3, 3, 0, 4, 3,
			4, 3, 0, 4, 1,
			5, 3, 0, 4, 0,
			6, 3, 0, 4, 2,
			7, 3, 0, 4, 1 }} ;//x, y, z, faceid, rotation


	//0=top, 1=bottom, 2=west, 3=east, 4=south, 5=north

	bool allrot = true;
	

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
	std::cout<<std::to_string(queue.enqueueNDRangeKernel(kernel, cl::NDRange(NULL), cl::NDRange(xlength, yrangesize, zlength)));

	if (allrot)
	{
		int *first = rotate90deg(&formation[0], facecount);

		

		int *second = rotate90deg(first, facecount);
		int *third = rotate90deg(second, facecount);

		/*for (int i = 0; i < facecount * 5; i++)
		{
			std::cout << third[i] << " ";
			if (i % 5 == 4)std::cout << std::endl;
		}*/

		cl::Buffer facesbuf2(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * arraysize, first);
		cl::Buffer facesbuf3(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * arraysize, second);
		cl::Buffer facesbuf4(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * arraysize, third);




		cl::Kernel kernel2(program, "kernelFile");
		kernel2.setArg(0, facesbuf2);
		kernel2.setArg(1, facecountbuf);
		kernel2.setArg(2, resultbuf);
		kernel2.setArg(3, xlengthbuf);
		kernel2.setArg(4, zlengthbuf);
		kernel2.setArg(5, yminbuf);

		std::cout << std::to_string(queue.enqueueNDRangeKernel(kernel2, cl::NDRange(NULL), cl::NDRange(xlength, yrangesize, zlength))) << std::endl;



		cl::Kernel kernel3(program, "kernelFile");
		kernel3.setArg(0, facesbuf3);
		kernel3.setArg(1, facecountbuf);
		kernel3.setArg(2, resultbuf);
		kernel3.setArg(3, xlengthbuf);
		kernel3.setArg(4, zlengthbuf);
		kernel3.setArg(5, yminbuf);

		std::cout << std::to_string(queue.enqueueNDRangeKernel(kernel3, cl::NDRange(NULL), cl::NDRange(xlength, yrangesize, zlength))) << std::endl;



		cl::Kernel kernel4(program, "kernelFile");
		kernel4.setArg(0, facesbuf4);
		kernel4.setArg(1, facecountbuf);
		kernel4.setArg(2, resultbuf);
		kernel4.setArg(3, xlengthbuf);
		kernel4.setArg(4, zlengthbuf);
		kernel4.setArg(5, yminbuf);

		std::cout << std::to_string(queue.enqueueNDRangeKernel(kernel4, cl::NDRange(NULL), cl::NDRange(xlength, yrangesize, zlength))) << std::endl;

		queue.finish();
	}

	queue.enqueueReadBuffer(resultbuf, CL_TRUE, 0, sizeof(int) * 3, loc.data());

	

	queue.finish();

	std::cout << "X: " << loc[0] << " Y: " << loc[1] << " Z: " << loc[2];


}

int *rotate90deg(int *formation, int facecount)//counterclockwise
{
	int *result = new int[facecount * 5];

	for (int i = 0; i < facecount; i++)
	{
		result[i * 5] = formation[i * 5 + 2];
		result[i * 5 + 1] = formation[i * 5 + 1];
		result[i * 5 + 2] = -1 * formation[i * 5];

		if (formation[i * 5 + 3] == 0) result[i * 5 + 3] = 0;
		if (formation[i * 5 + 3] == 1) result[i * 5 + 3] = 1;
		if (formation[i * 5 + 3] == 2) result[i * 5 + 3] = 4;
		if (formation[i * 5 + 3] == 3) result[i * 5 + 3] = 5;
		if (formation[i * 5 + 3] == 4) result[i * 5 + 3] = 3;
		if (formation[i * 5 + 3] == 5) result[i * 5 + 3] = 2;

		if (formation[i * 5 + 3] == 0)
		{
			if (formation[i * 5 + 4] == 0) result[i * 5 + 4] = 3;
			if (formation[i * 5 + 4] == 1) result[i * 5 + 4] = 0;
			if (formation[i * 5 + 4] == 2) result[i * 5 + 4] = 1;
			if (formation[i * 5 + 4] == 3) result[i * 5 + 4] = 2;
		}
		else if (formation[i * 5 + 3] == 1)
		{
			if (formation[i * 5 + 4] == 0) result[i * 5 + 4] = 1;
			if (formation[i * 5 + 4] == 1) result[i * 5 + 4] = 2;
			if (formation[i * 5 + 4] == 2) result[i * 5 + 4] = 3;
			if (formation[i * 5 + 4] == 3) result[i * 5 + 4] = 0;
		}
		else
		{
			result[i * 5 + 4] = formation[i * 5 + 4];
		}
	}

	return result;
}

