#include "vk_fun.h"

VkPipelineLayout gpipeline_layout_create(VkDevice *pDevice){
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		0,
		VK_NULL_HANDLE,
		0,
		VK_NULL_HANDLE
	};

	VkPipelineLayout pipelineLayout;
	vkCreatePipelineLayout(*pDevice, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &pipelineLayout);
	return pipelineLayout;
}

void gpipeline_layout_destroy(VkDevice *pDevice, VkPipelineLayout *pPipelineLayout){
	vkDestroyPipelineLayout(*pDevice, *pPipelineLayout, VK_NULL_HANDLE);
}

VkPipelineShaderStageCreateInfo shader_vertex_configure_stageCreateInfo(VkShaderModule *pVertexShaderModule, const char *entryName){
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		*pVertexShaderModule,
		entryName,
		VK_NULL_HANDLE
	};

	return vertexShaderStageCreateInfo;
}

VkPipelineShaderStageCreateInfo shader_fragment_configure_stageCreateInfo(VkShaderModule *pFragmentShaderModule, const char *entryName){
	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		*pFragmentShaderModule,
		entryName,
		VK_NULL_HANDLE
	};

	return fragmentShaderStageCreateInfo;
}

VkPipelineVertexInputStateCreateInfo shader_vertex_input_configure_stateCreateInfo(){
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		0,
		VK_NULL_HANDLE,
		0,
		VK_NULL_HANDLE
	};

	return vertexInputStateCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo gpipeline_input_assembly_configure_stateCreateInfo(){
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

	return inputAssemblyStateCreateInfo;
}

VkViewport gpipeline_viewport_configure(VkExtent2D *pExtent){
	VkViewport viewport = {
		1.0f,
		1.0f,
		pExtent->width,
		pExtent->height,
		0.0f,
		1.0f
	};

	return viewport;
}

VkRect2D gpipeline_scissor_configure(VkExtent2D *pExtent, uint32_t left, uint32_t right, uint32_t up, uint32_t down){
	if(left > pExtent->width){
		left = pExtent->width;
	}
	if(right > pExtent->width){
		right = pExtent->width;
	}
	if(up > pExtent->height){
		up = pExtent->height;
	}
	if(down > pExtent->height){
		down = pExtent->height;
	}
	VkOffset2D offset = {
		left,
		up
	};
	VkExtent2D extent = {
		pExtent->width - left - right,
		pExtent->height - up - down
	};
	VkRect2D scissor = {
		offset,
		extent
	};
	return scissor;
}

VkPipelineViewportStateCreateInfo gpipeline_viewport_configure_state_createInfo(VkViewport *pViewport, VkRect2D *pScissor){
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		1,
		pViewport,
		1,
		pScissor
	};

	return viewportStateCreateInfo;
}

VkPipelineRasterizationStateCreateInfo gpipeline_raterization_configure_state_createInfo(){
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	return rasterizationStateCreateInfo;
}

VkPipelineMultisampleStateCreateInfo gpipeline_multisample_configure_stateCreateInfo(){
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		VK_NULL_HANDLE,
		VK_FALSE,
		VK_FALSE
	};

	return multisampleStateCreateInfo;
}

VkPipelineColorBlendAttachmentState gpipeline_color_blend_configure_attachmentState(){
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	return colorBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo gpipeline_color_blend_configure_stateCreateInfo(VkPipelineColorBlendAttachmentState *pColorBlendAttachmentState){
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		pColorBlendAttachmentState,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	return colorBlendStateCreateInfo;
}

VkPipeline gpipeline_create(VkDevice *pDevice, VkPipelineLayout *pPipelineLayout, VkShaderModule *pVertexShaderModule, VkShaderModule *pFragmentShaderModule, VkRenderPass *pRenderPass, VkExtent2D *pExtent){
	char entryName[] = "main";

	VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {
			shader_vertex_configure_stageCreateInfo(pVertexShaderModule, entryName),
			shader_fragment_configure_stageCreateInfo(pFragmentShaderModule, entryName)
	};
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = shader_vertex_input_configure_stateCreateInfo();
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = gpipeline_input_assembly_configure_stateCreateInfo();
	VkViewport viewport = gpipeline_viewport_configure(pExtent);
	VkRect2D scissor = gpipeline_scissor_configure(pExtent, 0, 0, 0, 0);
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = gpipeline_viewport_configure_state_createInfo(&viewport,
                                                                                                              &scissor);
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = gpipeline_raterization_configure_state_createInfo();
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = gpipeline_multisample_configure_stateCreateInfo();
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = gpipeline_color_blend_configure_attachmentState();
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = gpipeline_color_blend_configure_stateCreateInfo(
            &colorBlendAttachmentState);

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		VK_NULL_HANDLE,
		0,
		2,
		shaderStageCreateInfo,
		&vertexInputStateCreateInfo,
		&inputAssemblyStateCreateInfo,
		VK_NULL_HANDLE,
		&viewportStateCreateInfo,
		&rasterizationStateCreateInfo,
		&multisampleStateCreateInfo,
		VK_NULL_HANDLE,
		&colorBlendStateCreateInfo,
		VK_NULL_HANDLE,
		*pPipelineLayout,
		*pRenderPass,
		0,
		VK_NULL_HANDLE,
		-1
	};

	VkPipeline graphicsPipeline;
	vkCreateGraphicsPipelines(*pDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, VK_NULL_HANDLE, &graphicsPipeline);
	return graphicsPipeline;
}

void gpipeline_destroy(VkDevice *pDevice, VkPipeline *pGraphicsPipeline){
	vkDestroyPipeline(*pDevice, *pGraphicsPipeline, VK_NULL_HANDLE);
}
