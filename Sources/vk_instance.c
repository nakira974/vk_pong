#include <setjmp.h>
#include "vk_fun.h"

jmp_buf exitJump;

/**
 * Private exception function
 */
void instance_exception() {
    longjmp(exitJump, 1);
}

bool checkValidationSupport(){
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, VK_NULL_HANDLE);
    VkLayerProperties  *layerProperties;
    if((layerProperties = (VkLayerProperties*) malloc(layer_count * sizeof(VkLayerProperties))) == NULL){
        free(layerProperties);
        log_write(ERR,"vk_instance", "VkValidationLayersException : Validation layers activated but unavailable!");
        return false;
    }

    vkEnumerateInstanceLayerProperties(&layer_count, layerProperties);
    for(int i =0;i<VALIDATION_LAYERS_COUNT;i++){
        const char * validation_layer_name = validationLayers[i];
        bool layer_found = false;
        for(int j=0;j<layer_count;j++){
            if (strcmp(validation_layer_name, layerProperties[j].layerName) == 0) {
                char line_buffer[256];
                sprintf(line_buffer, "Validation layer : %s has been found", validation_layer_name);
                log_write(DEBUG, "vk_instance",line_buffer);
                layer_found = true;
                break;
            }
        }
        if(!layer_found){
            free(layerProperties);
            return false;
        }
    }
    free(layerProperties);
    return true;
}

VkInstance createInstance(const char * app_name, uint32_t app_version, const char * engine_name, uint32_t engine_version){
    if (setjmp(exitJump) == 0) {

        if(enableValidationLayers){
            if (!checkValidationSupport()) {
                log_write(FATAL, "vk_instance", "VkValidationLayerException : Error, validations are activated but not available on the current platform!");
                instance_exception();
            }
        }
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
            log_write(FATAL, "vk_instance", "VkInstanceException : Error while creating vulkan instance!");
            instance_exception();
        };

        if(logLevel == DEBUG){
            if (setjmp(exitJump) == 0) {
                uint32_t extension_count = 0;
                vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extension_count, VK_NULL_HANDLE);
                VkExtensionProperties  *extensionProperties;
                if((extensionProperties = (VkExtensionProperties*) malloc(extension_count * sizeof(VkExtensionProperties))) == NULL){
                    free(extensionProperties);
                    log_write(ERR,"vk_instance", "VkExtensionException : Error while allocating extension properties default array!");
                    instance_exception();
                }

                if(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extension_count, extensionProperties) != VK_SUCCESS){
                    free(extensionProperties);
                    log_write(ERR, "vk_instance", "VkExtensionException : Error while allocating extension properties with Vulkan!");
                    instance_exception();
                };

                log_write(DEBUG, "vk_instance", "Available extensions :");


                for(int i =0;i<extension_count;i++){
                    char line_buffer[256];
                    sprintf(line_buffer,"Extension n°%d : %s", i+1, extensionProperties[i].extensionName);
                    log_write(DEBUG, "vk_instance", line_buffer);
                }

            }

        }


        return instance;
    }
    return VK_NULL_HANDLE;
}


void deleteInstance(VkInstance *pInstance){
	vkDestroyInstance(*pInstance, VK_NULL_HANDLE);
}
