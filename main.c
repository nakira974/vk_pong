/**
 * @file main.c
 * @brief This file contains the program main loop and entry point
 * @authors lonelydevil nakira974
 * @date 24/02/2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "std_c.h"
#include "ext.h"
#include "vk_fun.h"
#include "glfw_fun.h"

void signal_handler(int signal) {
    if(signal == SIGTERM){
        char line_buffer[256];
        sprintf(line_buffer, "End signal received : %d, program shutdown!", signal);
        log_write(ERR, "vk_pong", line_buffer);
        exit(1);
    }
}

LogLevel logLevel;
FILE * console;

int main(int argc, char * argv[]) {
    pthread_mutex_t main_thread_mutex = PTHREAD_MUTEX_INITIALIZER;

    // Default app log level
    logLevel =QUIET;
    if(argc>0 && argv != NULL){
        // Arguments processing
        log_process_args(argc, argv);
    }
    if(logLevel!=QUIET){
        // Création d'une nouvelle séquence de log
        console = fopen("console.log", "a");
        if(console != NULL){
            pthread_mutex_lock(&main_thread_mutex);
            fprintf(console,"\n");
            pthread_mutex_unlock(&main_thread_mutex);
            fclose(console);
        }
    }
    signal(SIGTERM, signal_handler);
    glfwInit();

    /**
     * ------------- Étape n°1 Instance et sélection du physical lDevice -------------
     */

    // Création de l'instance de notre app
    VkInstance instance = instance_create("vk_pong", VK_MAKE_VERSION(0, 0, 1), "NO ENGINE", VK_MAKE_VERSION(0, 0, 0));
    if(instance == VK_NULL_HANDLE)   raise(SIGTERM);
    uint32_t pDevices_count = pdevice_number(&instance);
    VkPhysicalDevice *pDevices = pdevice_get_devices(&instance, pDevices_count);
    // Selection d'une carte graphique compatible
    uint32_t best_pDevice_index = pdevice_get_best_index(pDevices, pDevices_count);
    // Création du physical lDevice et selection de la meilleure carte graphique compatible
    VkPhysicalDevice *best_pDevice = &pDevices[best_pDevice_index];

    /**
      * ------------- Étape n°2 Logical lDevice et famille de queues -------------
      */

    // Sélection d'une famille de queue compatible
    uint32_t queue_family_number = queue_get_family_number(best_pDevice);
    VkQueueFamilyProperties *queue_family_properties = queue_get_family_properties(best_pDevice, queue_family_number);
    // Création du logicial lDevice
    VkDevice lDevice = ldevice_create(best_pDevice, queue_family_number, queue_family_properties);
    // Sélection de la meilleure famille queue
    uint32_t bestGraphicsQueueFamilyindex = queue_get_best_graphics_family_index(queue_family_properties,
                                                                                 queue_family_number);
    uint32_t graphicsQueueMode = queue_get_graphics_mode(queue_family_properties, bestGraphicsQueueFamilyindex);
    // Création de queues pour nos différentes opérations asynchrones
    VkQueue drawingQueue = queue_get_drawing(&lDevice, bestGraphicsQueueFamilyindex);
    VkQueue presentingQueue = queue_get_presentation(&lDevice, bestGraphicsQueueFamilyindex, graphicsQueueMode);
    // Libération de la structure sur les properiétés des familles de queues de notre physical lDevice
    queue_delete_family_properties(&queue_family_properties);

    /**
   * ------------- Étape n°3 Création de la surface d'affichage et swap chain -------------
   */
    char windowTitle[] = "Vulkan Triangle";
    // Création d'une fenêtre de taille fixe 600 * 600 pixels
    GLFWwindow *window = window_create(600, 600, windowTitle);
    // Affectation de la fenêtre à la surface vulkan
    VkSurfaceKHR surface = surface_create(window, &instance);
    // Si notre physical lDevice supporte la surface on continue, sinon impossible d'utiliser Vulkan
    VkBool32 surfaceSupported = surface_get_support(&surface, best_pDevice, bestGraphicsQueueFamilyindex);
    if (!surfaceSupported) {
        log_write(FATAL, "vk_pong", "vulkan surface not supported!");

        // Ménage des références existantes
        surface_destroy(&surface, &instance);
        window_destroy(window);
        ldevice_destroy(&lDevice);
        pdevice_destroy(&pDevices);
        instance_destroy(&instance);
        pthread_mutex_destroy(&main_thread_mutex);

        raise(SIGTERM);
    }

    // Sélection des propriétés de la surface
    VkSurfaceCapabilitiesKHR surfaceCapabilities = surface_get_capabilities(&surface, best_pDevice);
    // Sélection du meilleur format pour la surface
    VkSurfaceFormatKHR bestSurfaceFormat = surface_get_best_format(&surface, best_pDevice);
    // Sélection du meilleur mode de présentation sur notre surface
    VkPresentModeKHR bestPresentMode = surface_get_best_present_mode(&surface, best_pDevice);
    // Sélection des meilleures extensions pour notre notre surface en fonction de ses capacités
    VkExtent2D bestSwapchainExtent = swapchain_get_best_extent(&surfaceCapabilities, window);
    uint32_t imageArrayLayers = 1;
    // Collection de cibles sur lesquelles nous pouvons effectuer un rendu
    VkSwapchainKHR swapchain = swapchain_create(&lDevice, &surface, &surfaceCapabilities, &bestSurfaceFormat,
                                                &bestSwapchainExtent, &bestPresentMode, imageArrayLayers,
                                                graphicsQueueMode);

    uint32_t swapchainImageNumber = swapchain_get_image_number(&lDevice, &swapchain);
    VkImage *swapchainImages = swapchain_get_images(&lDevice, &swapchain, swapchainImageNumber);

    /**
   * ------------- Étape n°4 Image views et frame buffers -------------
   */

    // Pour dessiner les images de la swapchain on doit l'encapsuler dans un VkImageView
    VkImageView *swapchainImageViews = imageviews_create(&lDevice, &swapchainImages, &bestSurfaceFormat,
                                                         swapchainImageNumber, imageArrayLayers);

    /**
  * ------------- Étape n5 Render passe -------------
  */
  // Création de la render passe pour décrire le type d'images utilisées et comment les traiter
    VkRenderPass renderPass = renderpass_create(&lDevice, &bestSurfaceFormat);
    // Encapsulation du VkImageView dans un VkFramebuffer
    VkFramebuffer *framebuffers = framebuffers_create(&lDevice, &renderPass, &bestSwapchainExtent, &swapchainImageViews,
                                                      swapchainImageNumber);

    /**
  * ------------- Étape n°6 Compilation des shaders en sprv et création du pipeline graphique -------------
  */
    uint32_t vertexShaderSize = 0;
    char vertexShaderFileName[] = "Shaders/triangle_vertex.spv";
    // Chargement du bytecode de notre vertex shader
    char *vertexShaderCode = shader_get_code(vertexShaderFileName, &vertexShaderSize);
    // Sortir du programme si le chargement a echoué
    if (vertexShaderCode == VK_NULL_HANDLE) {
        char line_buffer[256];
        sprintf(line_buffer, "VkShaderException : vertex %s shader not found!", vertexShaderFileName);
        log_write(FATAL, "vk_pong", line_buffer);

        framebuffers_destroy(&lDevice, &framebuffers, swapchainImageNumber);
        renderpass_destroy(&lDevice, &renderPass);
        imageviews_destroy(&lDevice, &swapchainImageViews, swapchainImageNumber);
        swapchain_destroy_images(&swapchainImages);
        swapchain_destroy(&lDevice, &swapchain);
        surface_destroy(&surface, &instance);
        window_destroy(window);
        ldevice_destroy(&lDevice);
        pdevice_destroy(&pDevices);
        instance_destroy(&instance);
        pthread_mutex_destroy(&main_thread_mutex);

        raise(SIGTERM);
    }
    // Création d'un module shader pour notre vertex shader
    VkShaderModule vertexShaderModule = shader_create_module(&lDevice, vertexShaderCode, vertexShaderSize);

    uint32_t fragmentShaderSize = 0;
    char fragmentShaderFileName[] = "Shaders/triangle_fragment.spv";
    // Chargement du bytecode de notre fragement shader
    char *fragmentShaderCode = shader_get_code(fragmentShaderFileName, &fragmentShaderSize);
    // Sortir du programme si le chargement a echoué
    if (fragmentShaderCode == VK_NULL_HANDLE) {
        char line_buffer[256];
        sprintf(line_buffer, "VkShaderException : fragment shader %s not found", fragmentShaderFileName);
        log_write(FATAL, "vk_pong", line_buffer);

        shader_destroy_module(&lDevice, &vertexShaderModule);
        shader_destroy_code(&vertexShaderCode);

        framebuffers_destroy(&lDevice, &framebuffers, swapchainImageNumber);
        renderpass_destroy(&lDevice, &renderPass);
        imageviews_destroy(&lDevice, &swapchainImageViews, swapchainImageNumber);
        swapchain_destroy_images(&swapchainImages);
        swapchain_destroy(&lDevice, &swapchain);
        surface_destroy(&surface, &instance);
        window_destroy(window);
        ldevice_destroy(&lDevice);
        pdevice_destroy(&pDevices);
        instance_destroy(&instance);
        pthread_mutex_destroy(&main_thread_mutex);

        raise(SIGTERM);
    }
    // Création d'un module shader pour notre fragment shader
    VkShaderModule fragmentShaderModule = shader_create_module(&lDevice, fragmentShaderCode, fragmentShaderSize);
    // Création d'un pipeline layout pour héberger nos pipelines graphique mais ici nous n'en avons qu'un seul
    VkPipelineLayout pipelineLayout = gpipeline_layout_create(&lDevice);
    // Création du pipeline graphique principal, on lui passe nos shader modules, sont layout, la render passe ainsi que les extensions de la swap chain
    VkPipeline graphicsPipeline = gpipeline_create(&lDevice, &pipelineLayout, &vertexShaderModule,
                                                   &fragmentShaderModule, &renderPass, &bestSwapchainExtent);

    // Une fois le byte code de nos shaders injecté dans le pipeline graphique on peut libérer les ressources
    // On retire à notre logical lDevice le module fragment shader
    shader_destroy_module(&lDevice, &fragmentShaderModule);
    // On supprime le byte code du fragment shader
    shader_destroy_code(&fragmentShaderCode);
    // On retire à notre logical lDevice le module vertex shader
    shader_destroy_module(&lDevice, &vertexShaderModule);
    // On supprime le byte code du vertex shader
    shader_destroy_code(&vertexShaderCode);

    /**
  * ------------- Étape n°7 Command Pool et Command Buffers -------------
  */

    // Allocateur principal pour nous command buffer
    VkCommandPool commandPool = command_pool_create(&lDevice, bestGraphicsQueueFamilyindex);
    // Allocation d'un command buffer à partir du pool
    VkCommandBuffer *commandBuffers = command_buffers_create(&lDevice, &commandPool, swapchainImageNumber);
    command_buffers_record(&commandBuffers, &renderPass, &framebuffers, &bestSwapchainExtent, &graphicsPipeline,
                           swapchainImageNumber);
    // Nombre maximum d'opérations authorisées sur les images
    uint32_t maxFrames = 2;
    // Création de sémaphore pour synchroniser la génération d'image et le rendu comme les command buffers sont asynchrones
    VkSemaphore *waitSemaphores = semaphores_create(&lDevice, maxFrames), *signalSemaphores = semaphores_create(&lDevice,
                                                                                                                maxFrames);
    // On créer des barrières qui servent à abriter le résultat de nos calculs sont les objects synchronisé entre les threads de génération et rendu
    VkFence *frontFences = fences_create(&lDevice, maxFrames), *backFences = fences_create_empty(swapchainImageNumber);

    /**
  * ------------- Étape n°8 Boucle principale -------------
  */
  // Boucle principale du programme
    main_loop(&lDevice, window, commandBuffers, frontFences, backFences, waitSemaphores, signalSemaphores,
              &swapchain,
              &drawingQueue, &presentingQueue, maxFrames);

    /**
  * ------------- Étape n°9 Gros ménage -------------
  */
    fences_destroy_empty(&backFences);
    fences_destroy(&lDevice, &frontFences, maxFrames);
    semaphores_destroy(&lDevice, &signalSemaphores, maxFrames);
    semaphores_destroy(&lDevice, &waitSemaphores, maxFrames);
    command_buffers_destroy(&lDevice, &commandBuffers, &commandPool, swapchainImageNumber);
    command_pool_destroy(&lDevice, &commandPool);
    gpipeline_destroy(&lDevice, &graphicsPipeline);
    gpipeline_layout_destroy(&lDevice, &pipelineLayout);
    framebuffers_destroy(&lDevice, &framebuffers, swapchainImageNumber);
    renderpass_destroy(&lDevice, &renderPass);
    imageviews_destroy(&lDevice, &swapchainImageViews, swapchainImageNumber);
    swapchain_destroy_images(&swapchainImages);
    swapchain_destroy(&lDevice, &swapchain);
    surface_destroy(&surface, &instance);
    window_destroy(window);
    ldevice_destroy(&lDevice);
    pdevice_destroy(&pDevices);
    instance_destroy(&instance);
    glfwTerminate();
    pthread_mutex_destroy(&main_thread_mutex);

    log_write(DEBUG, "vk_pong", "Program finished correctly");
    return 0;
}
