#ifndef VK_FUN_H
#define VK_FUN_H

#include "std_c.h"
#include "ext.h"

/**
 * @brief Create a Vulkan instance to link current application with API
 * @param app_name Application name
 * @param app_version Application version
 * @param engine_name Engine name
 * @param engine_version Engine version
 * @return The new VkInstance from the API
 */
VkInstance createInstance(const char * app_name,uint32_t app_version, const char * engine_name, uint32_t engine_version);

/**
 * @brief Delete a linked Vulkan instance from the API
 * @param pInstance The Vulkan instance to be removed
 */
void deleteInstance(VkInstance *pInstance);

/**
 * @brief Fetch the current count of physical devices
 * @param pInstance Application Vulkan instance
 * @return The physical devices list
 */
uint32_t getPhysicalDeviceNumber(VkInstance *pInstance);

/**
 * @brief Fetch Vulkan compatible physical devices in
 * @param pInstance Application Vulkan instance
 * @param physicalDeviceNumber Physical devices list
 * @return Vulkan compatible physical devices list
 */
VkPhysicalDevice *getPhysicalDevices(VkInstance *pInstance, uint32_t physicalDeviceNumber);

/**
 * @brief Remove a VkPhysicalDevice struct instance
 * @param ppPhysicalDevices Vulkan compatible physical devices list to be removed
 */
void deletePhysicalDevices(VkPhysicalDevice **ppPhysicalDevices);

/**
 * @brief Choose the best physical device
 * @param pPhysicalDevices Vulkan compatible physical devices list
 * @param physicalDeviceNumber The physical devices list
 * @return Index of the best physical device
 */
uint32_t getBestPhysicalDeviceIndex(VkPhysicalDevice *pPhysicalDevices, uint32_t physicalDeviceNumber);

/**
 * @brief Fetch the total amount of memory for a given physical device
 * @param pPhysicalDeviceMemoryProperties Physical device memory properties
 * @return Amount of memory available for the given device
 */
uint32_t getPhysicalDeviceTotalMemory(VkPhysicalDeviceMemoryProperties *pPhysicalDeviceMemoryProperties);

/**
 * @brief Fetch the list of supported queues family for a given physical device
 * @param pPhysicalDevice The physical device to get queues family on
 * @return The list of supported queues family for the given device
 */
uint32_t getqueueFamilyNumber(VkPhysicalDevice *pPhysicalDevice);

/**
 * @brief Fetch the queue family properties for a given device and a given queue family
 * @param pPhysicalDevice The physical device to inspect queue family details
 * @param queueFamilyNumber The queue family to inspect details on
 * @return The details of the given queue family for the given device
 */
VkQueueFamilyProperties *getQueueFamilyProperties(VkPhysicalDevice *pPhysicalDevice, uint32_t queueFamilyNumber);
void deleteQueueFamilyProperties(VkQueueFamilyProperties **ppQueueFamilyProperties);

/**
 * @brief Create a logical device
 * @param pPhysicalDevice Target physical device
 * @param queueFamilyNumber Chosen queue family
 * @param pQueueFamilyProperties Queue family properties for the given device and the chosen queue family
 * @return
 */
VkDevice createDevice(VkPhysicalDevice *pPhysicalDevice, uint32_t queueFamilyNumber, VkQueueFamilyProperties *pQueueFamilyProperties);

/**
 * @brief Destroy a logical device
 * @param pDevice The logical device to be destroyed
 */
void deleteDevice(VkDevice *pDevice);

/**
 * @brief Fetch the best graphics queue family for a given queue family
 * @param pQueueFamilyProperties Queue family properties for a given device and given family queue
 * @param queueFamilyNumber The queue family index
 * @return The chosen graphics family index
 */
uint32_t getBestGraphicsQueueFamilyindex(VkQueueFamilyProperties *pQueueFamilyProperties, uint32_t queueFamilyNumber);

/**
 * @brief Fetch the list of graphics queue mode for a given queue family
 * @param pQueueFamilyProperties Queue family properties for a given device and given family queue
 * @param graphicsQueueFamilyindex Chosen graphics family index for a given queue family
 * @return
 */
uint32_t getGraphicsQueueMode(VkQueueFamilyProperties *pQueueFamilyProperties, uint32_t graphicsQueueFamilyindex);

/**
 * @brief Allocate a queue for drawing operations
 * @param pDevice The target logical device where to allocate the queue
 * @param graphicsQueueFamilyindex Chosen queue family index
 * @return The application drawing queue
 */
VkQueue getDrawingQueue(VkDevice *pDevice, uint32_t graphicsQueueFamilyindex);

/**
 * @brief Allocate a queue for presenation operations
 * @param pDevice The target logical device where to allocate the queue
 * @param graphicsQueueFamilyindex Chosen queue family index
 * @param graphicsQueueMode Chosen graphics queue mode
 * @return The application presentation queue
 */
VkQueue getPresentingQueue(VkDevice *pDevice, uint32_t graphicsQueueFamilyindex, uint32_t graphicsQueueMode);

/**
 * @brief Create a Vulkan surface for a given GLFW window
 * @param pWindow The window to bet set in the Vulkan surface
 * @param pInstance Application Vulkan instance
 * @return The created Vulkan surface
 */
VkSurfaceKHR createSurface(GLFWwindow *pWindow, VkInstance *pInstance);

/**
 * @brief Destroy a Vulkan surface with its associated window
 * @param pSurface The Vulkan surface to be removed
 * @param pInstance Application Vulkan instance
 */
void deleteSurface(VkSurfaceKHR *pSurface, VkInstance *pInstance);

/**
 * @brief Fetch if the current platform does support Vulkan surface
 * @param pSurface The target surface to check if it's supported or not
 * @param pPhysicalDevice Physical device to lookup on
 * @param graphicsQueueFamilyindex Chosen queue family index
 * @return True if the physical device and the chosen family queue are supporting Vulkan surface, otherwise false
 */
VkBool32 getSurfaceSupport(VkSurfaceKHR *pSurface, VkPhysicalDevice *pPhysicalDevice, uint32_t graphicsQueueFamilyindex);

/**
 * @brief Fetch the current Vulkan surface capabilities (minimum length, supported colors...)
 * @param pSurface Target surface to get capabilities
 * @param pPhysicalDevice Target physical device where the surface is allocated
 * @return Current surface capabilities for the given physical device
 */
VkSurfaceCapabilitiesKHR getSurfaceCapabilities(VkSurfaceKHR *pSurface, VkPhysicalDevice *pPhysicalDevice);

/**
 * @brief Fetch the best surface format for rendering on a given surface
 * @param pSurface Target surface to get the best format on
 * @param pPhysicalDevice  Target physical device where the surface is allocated
 * @return The best surface format for the given surface
 */
VkSurfaceFormatKHR getBestSurfaceFormat(VkSurfaceKHR *pSurface, VkPhysicalDevice *pPhysicalDevice);

/**
 * @brief Fetch the best presentation mode for rendering on a given surface
 * @param pSurface Target surface to get the best presentation mode on
 * @param pPhysicalDevice Target physical device where is allocated the surface
 * @return The best present mode for the given surface
 */
VkPresentModeKHR getBestPresentMode(VkSurfaceKHR *pSurface, VkPhysicalDevice *pPhysicalDevice);

/**
 * @brief Fetch the best swapchain extent for a given surface capabilities and window
 * @param pSurfaceCapabilities Capabilities of the surface for the given window
 * @param window Target window of the swap chain
 * @return  The best swap chain extention for the given surface and window
 */
VkExtent2D getBestSwapchainExtent(VkSurfaceCapabilitiesKHR *pSurfaceCapabilities, GLFWwindow *window);

/**
 * Create a swapchain with the specified parameters
 *
 * @param pDevice Target logical device
 * @param pSurface Target surface
 * @param pSurfaceCapabilities - Target surface capabilities
 * @param pSurfaceFormat Target surface best format
 * @param pSwapchainExtent Chosen swap chain extensions for the given capabilities
 * @param pPresentMode Chosen surface presentation mode
 * @param imageArrayLayers Number of image array layers
 * @param graphicsQueueMode Chosen graphics queue mode for the given queue family
 * @return The created swap chain object
 */
VkSwapchainKHR createSwapChain(VkDevice *pDevice, VkSurfaceKHR *pSurface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities, VkSurfaceFormatKHR *pSurfaceFormat, VkExtent2D *pSwapchainExtent, VkPresentModeKHR *pPresentMode, uint32_t imageArrayLayers, uint32_t graphicsQueueMode);

/**
 * @brief Destroy the given swap chain
 * @param pDevice Target logical device
 * @param pSwapchain Target swap chain
 */
void deleteSwapchain(VkDevice *pDevice, VkSwapchainKHR *pSwapchain);

/**
 * @brief Fetch the number of images in a given swapchain
 * @param pDevice Target logical device
 * @param pSwapchain Target swap chain
 * @return The number current of images in the swapchain
 */
uint32_t getSwapchainImageNumber(VkDevice *pDevice, VkSwapchainKHR *pSwapchain);

/**
 * @brief Fetch an array of images in a given swapchain
 * @param pDevice Target logical device
 * @param pSwapchain Target swap chain
 * @param swapchainImageNumber Number of images in the swap chain
 * @return Pointer to the array of images in the swap chain
 */
VkImage *getSwapchainImages(VkDevice *pDevice, VkSwapchainKHR *pSwapchain, uint32_t swapchainImageNumber);

/**
 * @brief Delete the array of images in a given swap chain
 * @param ppImages Pointer to the pointer of the array of images
 */
void deleteSwapchainImages(VkImage **ppImages);

/**
 * @brief Create an array of image views for a given set of images
 * @param pDevice Target logical device
 * @param ppImages Pointer to the pointer of the array of images
 * @param pFormat Chosen surface format
 * @param imageNumber Number of images in the given array
 * @param imageArrayLayers Number of array layers for each image
 * @return Pointer to the array of image views
 */
VkImageView *createImageViews(VkDevice *pDevice, VkImage **ppImages, VkSurfaceFormatKHR *pFormat, uint32_t imageNumber, uint32_t imageArrayLayers);

/**
 * @brief Destroy an array of images view
 * @param pDevice Target logical device
 * @param ppImageViews Pointer to the pointer of the array of image view
 * @param imageViewNumber Number of image views in the given array
 */
void deleteImageViews(VkDevice *pDevice, VkImageView **ppImageViews, uint32_t imageViewNumber);

VkRenderPass createRenderPass(VkDevice *pDevice, VkSurfaceFormatKHR *pFormat);
void deleteRenderPass(VkDevice *pDevice, VkRenderPass *pRenderPass);
VkFramebuffer *createFramebuffers(VkDevice *pDevice, VkRenderPass *pRenderPass, VkExtent2D *pExtent, VkImageView **ppImageViews, uint32_t imageViewNumber);
void deleteFramebuffers(VkDevice *pDevice, VkFramebuffer **ppFramebuffers, uint32_t framebufferNumber);

char *getShaderCode(const char *fileName, uint32_t *pShaderSize);
void deleteShaderCode(char **ppShaderCode);
VkShaderModule createShaderModule(VkDevice *pDevice, char *pShaderCode, uint32_t shaderSize);
void deleteShaderModule(VkDevice *pDevice, VkShaderModule *pShaderModule);

VkPipelineLayout createPipelineLayout(VkDevice *pDevice);
void deletePipelineLayout(VkDevice *pDevice, VkPipelineLayout *pPipelineLayout);
VkPipelineShaderStageCreateInfo configureVertexShaderStageCreateInfo(VkShaderModule *pVertexShaderModule, const char *entryName);
VkPipelineShaderStageCreateInfo configureFragmentShaderStageCreateInfo(VkShaderModule *pFragmentShaderModule, const char *entryName);
VkPipelineVertexInputStateCreateInfo configureVertexInputStateCreateInfo();
VkPipelineInputAssemblyStateCreateInfo configureInputAssemblyStateCreateInfo();
VkViewport configureViewport(VkExtent2D *pExtent);
VkRect2D configureScissor(VkExtent2D *pExtent, uint32_t left, uint32_t right, uint32_t up, uint32_t down);
VkPipelineViewportStateCreateInfo configureViewportStateCreateInfo(VkViewport *pViewport, VkRect2D *pScissor);
VkPipelineRasterizationStateCreateInfo configureRasterizationStateCreateInfo();
VkPipelineMultisampleStateCreateInfo configureMultisampleStateCreateInfo();
VkPipelineColorBlendAttachmentState configureColorBlendAttachmentState();
VkPipelineColorBlendStateCreateInfo configureColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState *pColorBlendAttachmentState);
VkPipeline createGraphicsPipeline(VkDevice *pDevice, VkPipelineLayout *pPipelineLayout, VkShaderModule *pVertexShaderModule, VkShaderModule *pFragmentShaderModule, VkRenderPass *pRenderPass, VkExtent2D *pExtent);
void deleteGraphicsPipeline(VkDevice *pDevice, VkPipeline *pGraphicsPipeline);

VkCommandPool createCommandPool(VkDevice *pDevice, uint32_t queueFamilyIndex);
void deleteCommandPool(VkDevice *pDevice, VkCommandPool *pCommandPool);
VkCommandBuffer *createCommandBuffers(VkDevice *pDevice, VkCommandPool *pCommandPool, uint32_t commandBufferNumber);
void deleteCommandBuffers(VkDevice *pDevice, VkCommandBuffer **ppCommandBuffers, VkCommandPool *pCommandPool, uint32_t commandBufferNumber);
void recordCommandBuffers(VkCommandBuffer **ppCommandBuffers, VkRenderPass *pRenderPass, VkFramebuffer **ppFramebuffers, VkExtent2D *pExtent, VkPipeline *pPipeline, uint32_t commandBufferNumber);

VkSemaphore *createSemaphores(VkDevice *pDevice, uint32_t maxFrames);
void deleteSemaphores(VkDevice *pDevice, VkSemaphore **ppSemaphores, uint32_t maxFrames);
VkFence *createFences(VkDevice *pDevice, uint32_t maxFrames);
void deleteFences(VkDevice *pDevice, VkFence **ppFences, uint32_t maxFrames);
VkFence *createEmptyFences(uint32_t maxFrames);
void deleteEmptyFences(VkFence **ppFences);

/**
 * @brief Main program loop
 * @param pDevice Target logical device
 * @param window Target window
 * @param pCommandBuffers Target command buffers
 * @param pFrontFences Target presentation fences
 * @param pBackFences Target drawing fences
 * @param pWaitSemaphores  Target wait semaphores for a given number of frames
 * @param pSignalSemaphores Target signal semaphores for a given number of frames
 * @param pSwapchain Target swap chain
 * @param pDrawingQueue Target drawing queue
 * @param pPresentingQueue Target presentation queue
 * @param maxFrames The maximum number of frame per operation
 */
void presentImage(VkDevice *pDevice, GLFWwindow *window, VkCommandBuffer *pCommandBuffers, VkFence *pFrontFences, VkFence *pBackFences, VkSemaphore *pWaitSemaphores, VkSemaphore *pSignalSemaphores, VkSwapchainKHR *pSwapchain, VkQueue *pDrawingQueue, VkQueue *pPresentingQueue, uint32_t maxFrames);

void testLoop(GLFWwindow *window);

#endif // VK_FUN_H
