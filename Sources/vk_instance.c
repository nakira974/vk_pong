#include <setjmp.h>
#include "../Headers/vk_fun.h"

jmp_buf exitJump;

/**
 * Private exception function
 */
void instance_exception() {
    longjmp(exitJump, 1);
}

VkInstance createInstance(const char * app_name, uint32_t app_version, const char * engine_name, uint32_t engine_version){
    if (setjmp(exitJump) == 0) {
        VkApplicationInfo applicationInfo = {
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                VK_NULL_HANDLE,
                app_name,
                app_version,
                engine_name,
                engine_version,
                VK_API_VERSION_1_0
        };


        const char layerList[][VK_MAX_EXTENSION_NAME_SIZE] = {
                "VK_LAYER_KHRONOS_validation"
        };
        const char *layers[] = {
                layerList[0]
        };

        uint32_t extensionNumber = 0;
        const char *const *extensions = glfwGetRequiredInstanceExtensions(&extensionNumber);

        // Permet d’informer le driver des extensions et des validation layers
        // que nous utiliserons, et ceci de manière globale
        VkInstanceCreateInfo instanceCreateInfo = {
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                VK_NULL_HANDLE,
                0,
                &applicationInfo,
                1,
                layers,
                extensionNumber,
                extensions
        };

        VkInstance instance;
        if(vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &instance) != VK_SUCCESS){
            printf("VkInstanceException : Error while creating vulkan instance\n");
            instance_exception();
        };
        return instance;
    }
    return VK_NULL_HANDLE;
}


void deleteInstance(VkInstance *pInstance){
	vkDestroyInstance(*pInstance, VK_NULL_HANDLE);
}
