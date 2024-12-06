#ifdef __APPLE__
#include "OpenCL/opencl.hpp"
#else
#include "CL/opencl.hpp"
#endif

#include <iostream>
#include <stdexcept>

void lookup_devices() {
  cl::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (platforms.empty())
    throw std::runtime_error("No platforms found");

  std::cout << "Found " << platforms.size() << " platforms" << std::endl;
  for (auto &&platform : platforms) {
    std::string platformName;
    platform.getInfo(CL_PLATFORM_NAME, &platformName);

    cl::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    std::cout << "\tFound " << devices.size() << " devices for platform "
              << platformName << std::endl;

    for (auto &&device : devices) {
      std::string deviceName;
      device.getInfo(CL_DEVICE_NAME, &deviceName);
      std::cout << "\t\tDevice name: " << deviceName << std::endl;

      std::string deviceVendor;
      device.getInfo(CL_DEVICE_VENDOR, &deviceVendor);
      std::cout << "\t\tDevice vendor: " << deviceVendor << std::endl;

      std::string deviceExtensions;
      device.getInfo(CL_DEVICE_EXTENSIONS, &deviceExtensions);
      std::cout << "\t\tDevice extensions: " << deviceExtensions << std::endl;

      cl_ulong globalMemCacheSize;
      device.getInfo(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &globalMemCacheSize);
      std::cout << "\t\tDevice global memory cache size: " << globalMemCacheSize
                << std::endl;

      cl_ulong globalMemSize;
      device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemSize);
      std::cout << "\t\tDevice global memory size: " << globalMemSize
                << std::endl;

      cl_ulong localMemSize;
      device.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &localMemSize);
      std::cout << "\t\tDevice local memory size: " << localMemSize
                << std::endl;

      cl_uint addressBits;
      device.getInfo(CL_DEVICE_ADDRESS_BITS, &addressBits);
      std::cout << "\t\tDevice address space regime (bits): " << addressBits
                << std::endl;

      cl_uint maxClockFrequency;
      device.getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &maxClockFrequency);
      std::cout << "\t\tDevice max frequency: " << maxClockFrequency
                << std::endl;

      cl_uint maxComputeUnits;
      device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &maxComputeUnits);
      std::cout << "\t\tDevice max compute units: " << maxComputeUnits
                << std::endl;

      size_t maxWorkGroupSize;
      device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
      std::cout << "\t\tDevice max work group size: " << maxWorkGroupSize
                << std::endl;

      cl_uint maxWorkItemDimensions;
      device.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                     &maxWorkItemDimensions);
      std::cout << "\t\tDevice max work dimensions: " << maxWorkItemDimensions
                << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  try {
    lookup_devices();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
