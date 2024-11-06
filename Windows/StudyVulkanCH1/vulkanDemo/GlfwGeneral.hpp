#pragma once
#include "StudyVKBase.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib") //链接编译所需的静态库
//窗口的指针，全局变量自动初始化为NULL
GLFWwindow* pWindow;
//显示器信息的指针
GLFWmonitor* pMonitor;
//窗口标题
const char* windowTitle = "StudyVK";
using namespace std;
bool InitializeWindow(VkExtent2D size, bool fullScreen = false, bool isResizable = true, bool limitFrameRate = true) {
    std::cout << "InitializeWindow " << std::endl;
    using namespace vulkan;
	/*待后续填充*/
    if (!glfwInit()) {
        std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to initialize GLFW!\n");
        return false;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);
    pMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    pWindow = fullScreen ?
        glfwCreateWindow(pMode->width, pMode->height, windowTitle, pMonitor, nullptr) :
        glfwCreateWindow(size.width, size.height, windowTitle, nullptr, nullptr);
    if (!pWindow) {
        std::cout << std::format("[ InitializeWindow ]\nFailed to create a glfw window!\n");
        glfwTerminate();
        return false;
    }
#ifdef _WIN32
    if (0) { // for debug
        std::cout << "_WIN32 " << std::endl;
        uint32_t extensionCount = 0;
        const char** extensionNames;
        extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
        if (!extensionNames) {
            std::cout << std::format("[ InitializeWindow ]\nVulkan is not available on this machine!\n");
            glfwTerminate();
            return false;
        }
        for (size_t i = 0; i < extensionCount; i++)
            std::cout << "extensionNames[ " << i << "] :" << extensionNames[i] << std::endl;
    }
    // 在Windows上取得的扩展名称有两个，"VK_KHR_surface"和"VK_KHR_win32_surface"。
    graphicsBase::Base().AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    graphicsBase::Base().AddInstanceExtension("VK_KHR_win32_surface");
#else
    uint32_t extensionCount = 0;
    const char** extensionNames;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (!extensionNames) {
        std::cout << std::format("[ InitializeWindow ]\nVulkan is not available on this machine!\n");
        glfwTerminate();
        return false;
    }
    std::cout << "extensionNames : " << extensionNames << std::endl;

    for (size_t i = 0; i < extensionCount; i++)
        graphicsBase::Base().AddInstanceExtension(extensionNames[i]);
#endif
    graphicsBase::Base().AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkResult mVKR = graphicsBase::Base().UseLatestApiVersion();
    std::cout << "mVKR = " << mVKR << std::endl;
    if (graphicsBase::Base().CreateInstance())
        return false;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result = glfwCreateWindowSurface(vulkan::graphicsBase::Base().Instance(), pWindow, nullptr, &surface)) {
        std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to create a window surface!\nError code: {}\n", int32_t(result));
        glfwTerminate();
        return false;
    }
    graphicsBase::Base().Surface(surface);

    if (vulkan::graphicsBase::Base().GetPhysicalDevices() ||
        vulkan::graphicsBase::Base().DeterminePhysicalDevice(0, true, false) ||
        vulkan::graphicsBase::Base().CreateDevice())
        return false;

    if (graphicsBase::Base().CreateSwapchain(limitFrameRate))
        return false;
    return true;
}
void MakeWindowFullScreen() {
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, pMonitor, 0, 0, pMode->width, pMode->height, pMode->refreshRate);
}
void MakeWindowWindowed(VkOffset2D position, VkExtent2D size) {
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, nullptr, position.x, position.y, size.width, size.height, pMode->refreshRate);
}
void TerminateWindow() {
    vulkan::graphicsBase::Base().WaitIdle();
    glfwTerminate();
}
void TitleFps() {
    static double time0 = glfwGetTime();
    static double time1;
    static double dt;
    static int dframe = -1;
    static std::stringstream info;
    time1 = glfwGetTime();
    dframe++;
    if ((dt = time1 - time0) >= 1) {
        info.precision(1);
        info << windowTitle << "    " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(pWindow, info.str().c_str());
        info.str("");//别忘了在设置完窗口标题后清空所用的stringstream
        time0 = time1;
        dframe = 0;
    }
}