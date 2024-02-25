#include <setjmp.h>
#include "vk_fun.h"

jmp_buf exitJump;

/**
 * Private exception function
 */
void instance_exception() {
    longjmp(exitJump, 1);
}

bool instance_check_validation_support(){
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, VK_NULL_HANDLE);
    VkLayerProperties  *layerProperties;
    // Impossible d'allouer en mémoire un tableau de taille 'layer_count' elements
    if((layerProperties = (VkLayerProperties*) malloc(layer_count * sizeof(VkLayerProperties))) == NULL){
        free(layerProperties);
        log_write(ERR,"vk_instance", "VkValidationLayersException : Validation layers activated but unavailable!");
        return false;
    }
    // Allocation des élements du tableau layerProperties
    vkEnumerateInstanceLayerProperties(&layer_count, layerProperties);

    // Parcours des validation layers
    for(int i =0;i<VALIDATION_LAYERS_COUNT;i++){
        const char * validation_layer_name = validationLayers[i];
        bool layer_found = false;

        // Parcours des layer properties
        for(int j=0;j<layer_count;j++){
            // Si le validation layer de la plateforme match le validation layer courant on a trouvé notre layer
            if (strcmp(validation_layer_name, layerProperties[j].layerName) == 0) {
                char line_buffer[256];
                sprintf(line_buffer, "Validation layer : %s has been found", validation_layer_name);
                log_write(DEBUG, "vk_instance",line_buffer);
                layer_found = true;
                break;
            }
        }

        // Aucun validation layers trouvés
        if(!layer_found){
            free(layerProperties);
            return false;
        }
    }
    free(layerProperties);
    return true;
}

VkInstance instance_create(const char * app_name, uint32_t app_version, const char * engine_name, uint32_t engine_version){
    if (setjmp(exitJump) == 0) {

        // Si on est en build debug, on affiche les extensions supportées par la platforme courante
        if(enableValidationLayers){
            if (setjmp(exitJump) == 0) {
                uint32_t extension_count = 0;
                // On récupère le nombre d'extensions de la plateforme courante
                vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extension_count, VK_NULL_HANDLE);
                VkExtensionProperties  *extensionProperties;
                // Impossible d'allouer en mémoire un tableau de taille 'extension_count' elements
                if((extensionProperties = (VkExtensionProperties*) malloc(extension_count * sizeof(VkExtensionProperties))) == NULL){
                    free(extensionProperties);
                    log_write(ERR,"vk_instance", "VkExtensionException : Error while allocating extension properties default array!");
                    instance_exception();
                }
                // S'il est impossible d'allouer les élements du tableau extensionProperties, on soulève une exception
                if(vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extension_count, extensionProperties) != VK_SUCCESS){
                    free(extensionProperties);
                    log_write(ERR, "vk_instance", "VkExtensionException : Error while allocating extension properties with Vulkan!");
                    instance_exception();
                };

                // Afficher le nombre d'extensions disponibles
                char line_buffer[256];
                sprintf(line_buffer," %d available extensions", extension_count);
                log_write(DEBUG, "vk_instance", line_buffer);

                // Parcours des extensions de la platforme courante
                for(int i =0;i<extension_count;i++){
                    sprintf(line_buffer,"Extension n°%d : %s", i+1, extensionProperties[i].extensionName);
                    log_write(DEBUG, "vk_instance", line_buffer);
                }

            }

        }

        // Si on est en build debug on lance le check des validation layers LunarG du SDK
        if(enableValidationLayers){
            if (!instance_check_validation_support()) {
                log_write(FATAL, "vk_instance", "VkValidationLayerException : Error, validations are activated but not available on the current platform!");
                instance_exception();
            }
        }

        // Décrit notre application auprès de l'API
        VkApplicationInfo applicationInfo = {
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                VK_NULL_HANDLE,
                app_name,
                app_version,
                engine_name,
                engine_version,
                VK_API_VERSION_1_0
        };

        uint32_t extensionNumber = 0;
        // On récupère le nombre d'extensions GLFW spécifique à la plateforme

        const char *const *glfw_extensions;
        // Si on est en build de debug ajouter les extensions GLFW de debug
        if(enableValidationLayers){
            const char *const *glfw_instance_extensions = glfwGetRequiredInstanceExtensions(&extensionNumber);
            glfw_extensions = malloc((extensionNumber++) * sizeof (const char * const*));
            memcpy(glfw_extensions,glfw_instance_extensions, extensionNumber* sizeof(const char * const*) );
            // String temporaire pour copier la valeur d'extensions custom
            char* ext = malloc(strlen(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1);
            // On VK_EXT_DEBUG_UTILS_EXTENSION_NAME cette valeur dans le nouveau bloc de mémoire ext
            memcpy(ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, strlen(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1);
            // On calcul l'offset correct dans le tableau de pointeurs
            int offset = (extensionNumber-1) * sizeof(char*);
            // On y copie l'adresse du nouveau bloc de mémoire.
            memcpy((char*)glfw_extensions + offset, &ext, sizeof(const char *));
            // Le nouveau bloc d'adresse peut être affecté sans pb
            strcpy(glfw_extensions[extensionNumber-1], VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            log_write(DEBUG, "vk_instance", "GLFW debug extension enabled");

            char line_buffer[512];
            for(int i =0;i<extensionNumber;i++){
                if(glfw_extensions[i] != NULL) {
                    const char *current_extension = glfw_extensions[i];
                    sprintf(line_buffer, "GLFW extension n°%d : %s", i + 1, current_extension);
                } else {
                    sprintf(line_buffer, "GLFW extension n°%d : NULL", i + 1);
                }
                log_write(DEBUG, "vk_instance", line_buffer);
            }
        }else{
            glfw_extensions = glfwGetRequiredInstanceExtensions(&extensionNumber);
        }
        // Permet d’informer le driver des extensions et des validation layers
        // que nous utiliserons, et ceci de manière globale
        VkInstanceCreateInfo instanceCreateInfo = {
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                VK_NULL_HANDLE,
                0,
                &applicationInfo,
                1,
                validationLayers,
                extensionNumber,
                glfw_extensions
        };

        VkInstance instance;

        // S'il est impossible de créer notre instance, on soulève une exception
        if(vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &instance) != VK_SUCCESS){
            log_write(FATAL, "vk_instance", "VkInstanceException : Error while creating vulkan instance!");
            instance_exception();
        };

        return instance;
    }
    return VK_NULL_HANDLE;
}


void instance_destroy(VkInstance *pInstance){
	vkDestroyInstance(*pInstance, VK_NULL_HANDLE);
}
