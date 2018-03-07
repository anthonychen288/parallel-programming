#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>
#include <CL/cl.h>
#include <math.h>

const char* source = 
"__kernel\n"
"void histogramCL(__global unsigned int *img\n"
"				, __global unsigned int *result\n"
"				, int size)\n"
"{\n"
"	unsigned int idx = get_group_id(0)*get_local_size(0)+get_local_id(0);\n"
"	unsigned int ptrR, ptrG, ptrB;\n"
"	ptrR = result;ptrG = ptrR + 256; ptrB = ptrG + 256;\n"
"	if(idx %% 3 == 0)\n"
"	{\n"
"		ptrR[img[idx]]++;\n"
"	}else if(idx %% 3 == 1)\n"
"	{\n"
"		ptrG[img[idx]]++;\n"
"	}else\n"
"	{\n"
"		ptrB[img[idx]]++;\n"
"	}\n"
"	write_mem_fence(CLK_GLOBAL_MEM_FENCE);\n"
"}\n";


unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));
	
	cl_device_id *device;
	cl_platform_id *platform;
	cl_int errNum;
	cl_uint num_devs, num_plat;
	
	// step0:
	// get the number of platforms
	errNum = clGetPlatformIDs(0, NULL, &num_plat);
	printf("[info] Total %d platforms...\n", num_plat);
	// get the IDs of platforms
	platform = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_plat);
	errNum = clGetPlatformIDs(num_plat, platform, NULL);
	// get the number of devices
	errNum = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, 1, NULL, &num_devs);
	printf("[Info] Total %d devices...\n", num_devs);
	// get the IDs of devices
	device = (cl_device_id*) malloc(sizeof(cl_device_id)*num_devs);
	errNum = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, num_devs, device, NULL);
	if(errNum == CL_SUCCESS) printf("[Info] Get DeviceIDs Success!\n");  
    else fprintf(stderr, "[Error] Get DeviceIDs Fail! %d\n",  errNum);
	// declare context
	cl_context cntxt = clCreateContext(NULL, num_devs, device, NULL, NULL, &errNum);
	if(errNum == CL_SUCCESS) printf("[Info] Create Context Success!\n");  
    else fprintf(stderr, "[Error] Create Context Fail! %d\n",  errNum);
	// create cmd queue
	cl_command_queue cmdq = clCreateCommandQueue(cntxt, device[0], 0, &errNum);
	if(errNum == CL_SUCCESS) printf("[Info] Create Command Queue Success!\n");  
    else fprintf(stderr, "[Error] Create Command Queue Fail! %d\n", errNum);
	// step 1:
	//create buffer for input data & output data
	cl_mem imgd = clCreateBuffer(cntxt, CL_MEM_READ_ONLY, _size, NULL, &errNum);
	cl_mem result = clCreateBuffer(cntxt, CL_MEM_WRITE_ONLY, 256 * 3 * sizeof(unsigned int), NULL, &errNum);
	//transfering data
	errNum = clEnqueueWriteBuffer(cmdq, imgd, CL_TRUE, 0, _size, img, 0, NULL, NULL);
	errNum = clEnqueueWriteBuffer(cmdq, result, CL_TRUE, 0
			, 256 * 3 * sizeof(unsigned int), ref_histogram_results, 0, NULL, NULL);
	//step 2:
	// create program
	cl_program prog = clCreateProgramWithSource(cntxt, 1, (const char **)&source, NULL, &errNum);
	if(errNum == CL_SUCCESS) printf("[Info] Create Program Success!\n");  
    else fprintf(stderr, "[Error] Create Program Fail! %d\n", errNum); 
	// build
	errNum = clBuildProgram(prog, num_devs, device, NULL, NULL, NULL);
	if(errNum == CL_SUCCESS) printf("[Info] Build Program Success!\n");  
    else fprintf(stderr, "[Error] Build Program Fail!\n");
	// select kernel
	cl_kernel mykernel = clCreateKernel(prog, "histogramCL", &errNum);
	//step 3:
	// set arguments
	clSetKernelArg(mykernel, 0, sizeof(cl_mem), (void*)&img);
	clSetKernelArg(mykernel, 1, sizeof(cl_mem), (void*)&result);
	clSetKernelArg(mykernel, 2, sizeof(unsigned int), (void*)&_size);
	// set workgroup sizes
	size_t locws[1] = {1024};
	size_t globws[1] = {floor(_size/1024)};
	// size_t locws[2] = {width, width};
	// execute kernel
	clEnqueueNDRangeKernel(cmdq, mykernel, 1, NULL, globws, locws, 0, NULL, NULL);
	// step 4
	// read back
	clEnqueueReadBuffer(cmdq, result, CL_TRUE, 0
			, 256 * 3 * sizeof(unsigned int), ref_histogram_results, NULL, NULL, NULL);
	
	clReleaseProgram(prog);
	clReleaseKernel(mykernel);
	clReleaseMemObject(imgd);
	clReleaseMemObject(result);
	clReleaseContext(cntxt);
	clReleaseCommandQueue(cmdq);
	
	free(device);free(platform);
	/*
	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}*/

	return ref_histogram_results;
}

int main(int argc, char const *argv[])
{

	unsigned int * histogram_results;
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0556097.out", std::ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}

	histogram_results = histogram(image, input_size);
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();

	return 0;
}