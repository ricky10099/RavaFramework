#pragma once
// #include "RavaFramework.h"

static constexpr int MAX_FRAMES_SYNC = 2;

const std::vector<const char*> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

namespace VK {
//static VkCommandBuffer BeginSingleTimeCommands() {
//  // Command Buffer details
//  VkCommandBufferAllocateInfo allocInfo{};
//  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//  allocInfo.commandPool        = Renderer::Instance->GetContext()->GetCommandPool();
//  allocInfo.commandBufferCount = 1;
//
//  // Command buffer to hold transfer commands
//  VkCommandBuffer commandBuffer;
//  vkAllocateCommandBuffers(VKContext->GetLogicalDevice(), &allocInfo, &commandBuffer);
//
//  // Information to begin command buffer record
//  VkCommandBufferBeginInfo beginInfo{};
//  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//  // Begin recording transfer commands
//  vkBeginCommandBuffer(commandBuffer, &beginInfo);
//  return commandBuffer;
//}
}  // namespace VK