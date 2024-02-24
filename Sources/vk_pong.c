#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../Headers/std_c.h"
#include "../Headers/ext.h"
#include "../Headers/vk_fun.h"
#include "../Headers/glfw_fun.h"

void signal_handler(int signal) {
    if(signal == SIGTERM){
        printf("End singla received : %d, program shutdown!\n", signal);
        exit(1);
    }
}

int main() {
    signal(SIGTERM, signal_handler);
    glfwInit();

    /**
     * ------------- Étape n°1 Instance et sélection du physical device -------------
     */

    // Création de l'instance de notre app
    VkInstance instance = createInstance("vk_pong", VK_MAKE_VERSION(0, 0, 1), "NO ENGINE", VK_MAKE_VERSION(0, 0, 0));
    if(instance == VK_NULL_HANDLE)   raise(SIGTERM);
    uint32_t physicalDeviceNumber = getPhysicalDeviceNumber(&instance);
    VkPhysicalDevice *physicalDevices = getPhysicalDevices(&instance, physicalDeviceNumber);
    // Selection d'une carte graphique compatible
    uint32_t bestPhysicalDeviceIndex = getBestPhysicalDeviceIndex(physicalDevices, physicalDeviceNumber);
    // Création du physical device et selection de la meilleure carte graphique compatible
    VkPhysicalDevice *pBestPhysicalDevice = &physicalDevices[bestPhysicalDeviceIndex];

    /**
      * ------------- Étape n°2 Logical device et famille de queues -------------
      */

    // Sélection d'une famille de queue compatible
    uint32_t queueFamilyNumber = getQueueFamilyNumber(pBestPhysicalDevice);
    VkQueueFamilyProperties *queueFamilyProperties = getQueueFamilyProperties(pBestPhysicalDevice, queueFamilyNumber);
    // Création du logicial device
    VkDevice device = createDevice(pBestPhysicalDevice, queueFamilyNumber, queueFamilyProperties);
    // Sélection de la meilleure famille queue
    uint32_t bestGraphicsQueueFamilyindex = getBestGraphicsQueueFamilyindex(queueFamilyProperties, queueFamilyNumber);
    uint32_t graphicsQueueMode = getGraphicsQueueMode(queueFamilyProperties, bestGraphicsQueueFamilyindex);
    // Création de queues pour nos différentes opérations asynchrones
    VkQueue drawingQueue = getDrawingQueue(&device, bestGraphicsQueueFamilyindex);
    VkQueue presentingQueue = getPresentingQueue(&device, bestGraphicsQueueFamilyindex, graphicsQueueMode);
    // Libération de la structure sur les properiétés des familles de queues de notre physical device
    deleteQueueFamilyProperties(&queueFamilyProperties);

    /**
   * ------------- Étape n°3 Création de la surface d'affichage et swap chain -------------
   */
    char windowTitle[] = "Vulkan Triangle";
    // Création d'une fenêtre de taille fixe 600 * 600 pixels
    GLFWwindow *window = createVulkanWindow(600, 600, windowTitle);
    // Affectation de la fenêtre à la surface vulkan
    VkSurfaceKHR surface = createSurface(window, &instance);
    // Si notre physical device supporte la surface on continue, sinon impossible d'utiliser Vulkan
    VkBool32 surfaceSupported = getSurfaceSupport(&surface, pBestPhysicalDevice, bestGraphicsQueueFamilyindex);
    if (!surfaceSupported) {
        printf("vulkan surface not supported!\n");

        // Ménage des références existantes
        deleteSurface(&surface, &instance);
        deleteWindow(window);
        deleteDevice(&device);
        deletePhysicalDevices(&physicalDevices);
        deleteInstance(&instance);

        raise(SIGTERM);
    }

    // Sélection des propriétés de la surface
    VkSurfaceCapabilitiesKHR surfaceCapabilities = getSurfaceCapabilities(&surface, pBestPhysicalDevice);
    // Sélection du meilleur format pour la surface
    VkSurfaceFormatKHR bestSurfaceFormat = getBestSurfaceFormat(&surface, pBestPhysicalDevice);
    // Sélection du meilleur mode de présentation sur notre surface
    VkPresentModeKHR bestPresentMode = getBestPresentMode(&surface, pBestPhysicalDevice);
    // Sélection des meilleures extensions pour notre notre surface en fonction de ses capacités
    VkExtent2D bestSwapchainExtent = getBestSwapchainExtent(&surfaceCapabilities, window);
    uint32_t imageArrayLayers = 1;
    // Collection de cibles sur lesquelles nous pouvons effectuer un rendu
    VkSwapchainKHR swapchain = createSwapChain(&device, &surface, &surfaceCapabilities, &bestSurfaceFormat,
                                               &bestSwapchainExtent, &bestPresentMode, imageArrayLayers,
                                               graphicsQueueMode);

    uint32_t swapchainImageNumber = getSwapchainImageNumber(&device, &swapchain);
    VkImage *swapchainImages = getSwapchainImages(&device, &swapchain, swapchainImageNumber);

    /**
   * ------------- Étape n°4 Image views et frame buffers -------------
   */

    // Pour dessiner les images de la swapchain on doit l'encapsuler dans un VkImageView
    VkImageView *swapchainImageViews = createImageViews(&device, &swapchainImages, &bestSurfaceFormat,
                                                        swapchainImageNumber, imageArrayLayers);

    /**
  * ------------- Étape n5 Render passe -------------
  */
  // Création de la render passe pour décrire le type d'images utilisées et comment les traiter
    VkRenderPass renderPass = createRenderPass(&device, &bestSurfaceFormat);
    // Encapsulation du VkImageView dans un VkFramebuffer
    VkFramebuffer *framebuffers = createFramebuffers(&device, &renderPass, &bestSwapchainExtent, &swapchainImageViews,
                                                     swapchainImageNumber);

    /**
  * ------------- Étape n°6 Compilation des shaders en sprv et création du pipeline graphique -------------
  */
    uint32_t vertexShaderSize = 0;
    char vertexShaderFileName[] = "Shaders/triangle_vertex.spv";
    // Chargement du bytecode de notre vertex shader
    char *vertexShaderCode = getShaderCode(vertexShaderFileName, &vertexShaderSize);
    // Sortir du programme si le chargement a echoué
    if (vertexShaderCode == VK_NULL_HANDLE) {
        printf("VkShaderException : vertex %s shader not found!", vertexShaderFileName);

        deleteFramebuffers(&device, &framebuffers, swapchainImageNumber);
        deleteRenderPass(&device, &renderPass);
        deleteImageViews(&device, &swapchainImageViews, swapchainImageNumber);
        deleteSwapchainImages(&swapchainImages);
        deleteSwapchain(&device, &swapchain);
        deleteSurface(&surface, &instance);
        deleteWindow(window);
        deleteDevice(&device);
        deletePhysicalDevices(&physicalDevices);
        deleteInstance(&instance);

        raise(SIGTERM);
    }
    // Création d'un module shader pour notre vertex shader
    VkShaderModule vertexShaderModule = createShaderModule(&device, vertexShaderCode, vertexShaderSize);

    uint32_t fragmentShaderSize = 0;
    char fragmentShaderFileName[] = "Shaders/triangle_fragment.spv";
    // Chargement du bytecode de notre fragement shader
    char *fragmentShaderCode = getShaderCode(fragmentShaderFileName, &fragmentShaderSize);
    // Sortir du programme si le chargement a echoué
    if (fragmentShaderCode == VK_NULL_HANDLE) {
        printf("VkShaderException : fragment shader %s not found", fragmentShaderFileName);

        deleteShaderModule(&device, &vertexShaderModule);
        deleteShaderCode(&vertexShaderCode);

        deleteFramebuffers(&device, &framebuffers, swapchainImageNumber);
        deleteRenderPass(&device, &renderPass);
        deleteImageViews(&device, &swapchainImageViews, swapchainImageNumber);
        deleteSwapchainImages(&swapchainImages);
        deleteSwapchain(&device, &swapchain);
        deleteSurface(&surface, &instance);
        deleteWindow(window);
        deleteDevice(&device);
        deletePhysicalDevices(&physicalDevices);
        deleteInstance(&instance);

        return 1;
    }
    // Création d'un module shader pour notre fragment shader
    VkShaderModule fragmentShaderModule = createShaderModule(&device, fragmentShaderCode, fragmentShaderSize);
    // Création d'un pipeline layout pour héberger nos pipelines graphique mais ici nous n'en avons qu'un seul
    VkPipelineLayout pipelineLayout = createPipelineLayout(&device);
    // Création du pipeline graphique principal, on lui passe nos shader modules, sont layout, la render passe ainsi que les extensions de la swap chain
    VkPipeline graphicsPipeline = createGraphicsPipeline(&device, &pipelineLayout, &vertexShaderModule,
                                                         &fragmentShaderModule, &renderPass, &bestSwapchainExtent);

    // Une fois le byte code de nos shaders injecté dans le pipeline graphique on peut libérer les ressources
    // On retire à notre logical device le module fragment shader
    deleteShaderModule(&device, &fragmentShaderModule);
    // On supprime le byte code du fragment shader
    deleteShaderCode(&fragmentShaderCode);
    // On retire à notre logical device le module vertex shader
    deleteShaderModule(&device, &vertexShaderModule);
    // On supprime le byte code du vertex shader
    deleteShaderCode(&vertexShaderCode);

    /**
  * ------------- Étape n°7 Command Pool et Command Buffers -------------
  */

    // Allocateur principal pour nous command buffer
    VkCommandPool commandPool = createCommandPool(&device, bestGraphicsQueueFamilyindex);
    // Allocation d'un command buffer à partir du pool
    VkCommandBuffer *commandBuffers = createCommandBuffers(&device, &commandPool, swapchainImageNumber);
    recordCommandBuffers(&commandBuffers, &renderPass, &framebuffers, &bestSwapchainExtent, &graphicsPipeline,
                         swapchainImageNumber);
    // Nombre maximum d'opérations authorisées sur les images
    uint32_t maxFrames = 2;
    // Création de sémaphore pour synchroniser la génération d'image et le rendu comme les command buffers sont asynchrones
    VkSemaphore *waitSemaphores = createSemaphores(&device, maxFrames), *signalSemaphores = createSemaphores(&device,maxFrames);
    // On créer des barrières qui servent à abriter le résultat de nos calculs sont les objects synchronisé entre les threads de génération et rendu
    VkFence *frontFences = createFences(&device, maxFrames), *backFences = createEmptyFences(swapchainImageNumber);

    /**
  * ------------- Étape n°8 Boucle principale -------------
  */
  // Boucle principal du programme
    presentImage(&device, window, commandBuffers, frontFences, backFences, waitSemaphores, signalSemaphores, &swapchain,
                 &drawingQueue, &presentingQueue, maxFrames);

    /**
  * ------------- Étape n°9 Gros ménage -------------
  */
    deleteEmptyFences(&backFences);
    deleteFences(&device, &frontFences, maxFrames);
    deleteSemaphores(&device, &signalSemaphores, maxFrames);
    deleteSemaphores(&device, &waitSemaphores, maxFrames);
    deleteCommandBuffers(&device, &commandBuffers, &commandPool, swapchainImageNumber);
    deleteCommandPool(&device, &commandPool);
    deleteGraphicsPipeline(&device, &graphicsPipeline);
    deletePipelineLayout(&device, &pipelineLayout);
    deleteFramebuffers(&device, &framebuffers, swapchainImageNumber);
    deleteRenderPass(&device, &renderPass);
    deleteImageViews(&device, &swapchainImageViews, swapchainImageNumber);
    deleteSwapchainImages(&swapchainImages);
    deleteSwapchain(&device, &swapchain);
    deleteSurface(&surface, &instance);
    deleteWindow(window);
    deleteDevice(&device);
    deletePhysicalDevices(&physicalDevices);
    deleteInstance(&instance);

    glfwTerminate();
    return 0;
}
