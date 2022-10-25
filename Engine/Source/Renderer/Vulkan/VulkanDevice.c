#include <Nara/Containers/DynArray.h>
#include <Nara/Containers/String.h>
#include <Nara/Core/Log.h>
#include <Nara/Core/Memory.h>
#include <Nara/Renderer/Vulkan/VulkanDevice.h>

typedef struct VulkanDevice_QueueSearchContext {
	U32 FamilyCount;
	VkQueueFamilyProperties* FamilyProperties;
	float** FamilyPriorities;
	U32* NextFamilyIndex;
} VulkanDevice_QueueSearchContext;

B8 VulkanDevice_AssignQueue(VulkanDevice_QueueSearchContext* context,
                            Vulkan_QueueType type,
                            VkQueueFlags required,
                            VkQueueFlags ignored);
B8 VulkanDevice_CreateDevice();
B8 VulkanDevice_FindPhysicalDevice();

void Vulkan_LoadDevice() {
	Vulkan->vk.GetDeviceProcAddr =
		(PFN_vkGetDeviceProcAddr) Vulkan->vk.GetInstanceProcAddr(Vulkan->Instance, "vkGetDeviceProcAddr");

#define Load(fn) Vulkan->vk.fn = (PFN_vk##fn) Vulkan->vk.GetDeviceProcAddr(Vulkan->Device->Device, "vk" #fn)
	Load(DestroyDevice);
	Load(GetDeviceQueue);
#undef Load
}

B8 VulkanDevice_Initialize() {
	Vulkan->Device = Allocate(sizeof(*Vulkan->Device), MemoryTag_Renderer);
	if (!Vulkan->Device) { return FALSE; }
	MemZero(Vulkan->Device, sizeof(*Vulkan->Device));

	if (!VulkanDevice_FindPhysicalDevice()) {
		LogF("[Vulkan] Failed to find a suitable physical device!");
		return FALSE;
	}
	LogD("[Vulkan] Using physical device: %s", Vulkan->DeviceInfo.Properties.Properties.deviceName);

	if (!VulkanDevice_CreateDevice()) {
		LogF("[Vulkan] Failed to create logical device!");
		return FALSE;
	}

	return TRUE;
}

void VulkanDevice_Shutdown() {
	if (!Vulkan->Device) { return; }

	Vulkan->vk.DestroyDevice(Vulkan->Device->Device, &Vulkan->Allocator);
	Free(Vulkan->Device);
	DynArray_Destroy(Vulkan->DeviceInfo.EnabledExtensions);
	DynArray_Destroy(Vulkan->DeviceInfo.QueueFamilies);
	DynArray_Destroy(Vulkan->DeviceInfo.AvailableExtensions);
}

B8 VulkanDevice_FindPhysicalDevice() {
	U32 physicalDeviceCount = 0;
	VkAssert(Vulkan->vk.EnumeratePhysicalDevices(Vulkan->Instance, &physicalDeviceCount, NULL));
	LogT("[Vulkan] Found %u physical devices.", physicalDeviceCount);
	if (physicalDeviceCount == 0) { return FALSE; }

	VkPhysicalDevice* physicalDevices = DynArray_CreateWithSize(VkPhysicalDevice, physicalDeviceCount);
	VkAssert(Vulkan->vk.EnumeratePhysicalDevices(Vulkan->Instance, &physicalDeviceCount, physicalDevices));

	for (int i = 0; i < physicalDeviceCount; ++i) {
		const VkPhysicalDevice physicalDevice = physicalDevices[i];
		VulkanDevice_Info deviceInfo;
		MemZero(&deviceInfo, sizeof(deviceInfo));

		U32 extensionCount = 0;
		VkAssert(Vulkan->vk.EnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL));
		deviceInfo.AvailableExtensions = DynArray_CreateWithSize(VkExtensionProperties, extensionCount);
		VkAssert(Vulkan->vk.EnumerateDeviceExtensionProperties(
			physicalDevice, NULL, &extensionCount, deviceInfo.AvailableExtensions));

		Vulkan->vk.GetPhysicalDeviceMemoryProperties(physicalDevice, &deviceInfo.Memory);

		// Features enumeration
		{
			VkPhysicalDeviceFeatures2 features2 = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = NULL};
			VkPhysicalDeviceMaintenance4Features maintenance4 = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES, .pNext = NULL};
			VkPhysicalDeviceSynchronization2Features synchronization2 = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES, .pNext = NULL};
			VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParameter = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES, .pNext = NULL};
			VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphore = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES, .pNext = NULL};

			void** next = &features2.pNext;
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
				*next = &maintenance4;
				next  = &maintenance4.pNext;
			}
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
				*next = &synchronization2;
				next  = &synchronization2.pNext;
			}
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME)) {
				*next = &shaderDrawParameter;
				next  = &shaderDrawParameter.pNext;
			}
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
				*next = &timelineSemaphore;
				next  = &timelineSemaphore.pNext;
			}

			Vulkan->vk.GetPhysicalDeviceFeatures2(physicalDevice, &features2);

			MemCopy(&deviceInfo.AvailableFeatures.Features, &features2.features, sizeof(features2.features));
			MemCopy(&deviceInfo.AvailableFeatures.Maintenance4, &maintenance4, sizeof(maintenance4));
			MemCopy(&deviceInfo.AvailableFeatures.Synchronization2, &synchronization2, sizeof(synchronization2));
			MemCopy(&deviceInfo.AvailableFeatures.ShaderDrawParameters, &shaderDrawParameter, sizeof(shaderDrawParameter));
			MemCopy(&deviceInfo.AvailableFeatures.TimelineSemaphore, &timelineSemaphore, sizeof(timelineSemaphore));
		}

		// Properties enumeration
		{
			VkPhysicalDeviceProperties2 properties2 = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
			                                           .pNext = NULL};
			VkPhysicalDeviceDriverProperties driver = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
			                                           .pNext = NULL};
			VkPhysicalDeviceMaintenance4Properties maintenance4 = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES, .pNext = NULL};
			VkPhysicalDeviceTimelineSemaphoreProperties timelineSemaphore = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES, .pNext = NULL};

			void** next = &properties2.pNext;
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
				*next = &driver;
				next  = &driver.pNext;
			}
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
				*next = &maintenance4;
				next  = &maintenance4.pNext;
			}
			if (Vulkan_ExtensionPresent(deviceInfo.AvailableExtensions, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
				*next = &timelineSemaphore;
				next  = &timelineSemaphore.pNext;
			}

			Vulkan->vk.GetPhysicalDeviceProperties2(physicalDevice, &properties2);

			MemCopy(&deviceInfo.Properties.Properties, &properties2.properties, sizeof(properties2.properties));
			MemCopy(&deviceInfo.Properties.Driver, &driver, sizeof(driver));
			MemCopy(&deviceInfo.Properties.Maintenance4, &maintenance4, sizeof(maintenance4));
			MemCopy(&deviceInfo.Properties.TimelineSemaphore, &timelineSemaphore, sizeof(timelineSemaphore));
		}

		U32 familyCount = 0;
		Vulkan->vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, NULL);
		deviceInfo.QueueFamilies = DynArray_CreateWithSize(VkQueueFamilyProperties, familyCount);
		Vulkan->vk.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, deviceInfo.QueueFamilies);

		if (TRUE) {
			// TODO: Verify device capabilities, sort by best device.
			Vulkan->Device->PhysicalDevice = physicalDevice;
			MemCopy(&Vulkan->DeviceInfo, &deviceInfo, sizeof(deviceInfo));
			break;
		} else {
			DynArray_Destroy(deviceInfo.QueueFamilies);
			DynArray_Destroy(deviceInfo.AvailableExtensions);
		}
	}

	DynArray_Destroy(physicalDevices);

	return Vulkan->Device->PhysicalDevice != VK_NULL_HANDLE;
}

B8 VulkanDevice_CreateDevice() {
	Vulkan->DeviceInfo.EnabledExtensions = DynArray_Create(const char*);

	Vulkan->Extensions.CalibratedTimestamps = Vulkan_TryExtension(Vulkan->DeviceInfo.AvailableExtensions,
	                                                              Vulkan->DeviceInfo.EnabledExtensions,
	                                                              VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
	Vulkan->Extensions.Maintenance4         = Vulkan_TryExtension(
    Vulkan->DeviceInfo.AvailableExtensions, Vulkan->DeviceInfo.EnabledExtensions, VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
	Vulkan->Extensions.Swapchain = Vulkan_TryExtension(
		Vulkan->DeviceInfo.AvailableExtensions, Vulkan->DeviceInfo.EnabledExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	Vulkan->Extensions.Synchronization2  = Vulkan_TryExtension(Vulkan->DeviceInfo.AvailableExtensions,
                                                            Vulkan->DeviceInfo.EnabledExtensions,
                                                            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
	Vulkan->Extensions.TimelineSemaphore = Vulkan_TryExtension(Vulkan->DeviceInfo.AvailableExtensions,
	                                                           Vulkan->DeviceInfo.EnabledExtensions,
	                                                           VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);

	for (int i = 0; i < Vulkan_QueueType_MAX; ++i) {
		Vulkan->Queues[i].Family = VK_QUEUE_FAMILY_IGNORED;
		Vulkan->Queues[i].Index  = VK_QUEUE_FAMILY_IGNORED;
		Vulkan->Queues[i].Queue  = VK_NULL_HANDLE;
	}

	VulkanDevice_QueueSearchContext searchContext = {};
	searchContext.FamilyCount                     = DynArray_Size(Vulkan->DeviceInfo.QueueFamilies);
	searchContext.FamilyProperties = DynArray_CreateWithSize(VkQueueFamilyProperties, searchContext.FamilyCount);
	MemCopy(searchContext.FamilyProperties,
	        Vulkan->DeviceInfo.QueueFamilies,
	        sizeof(VkQueueFamilyProperties) * searchContext.FamilyCount);
	searchContext.FamilyPriorities = DynArray_CreateWithSize(float*, searchContext.FamilyCount);
	for (int i = 0; i < searchContext.FamilyCount; ++i) { searchContext.FamilyPriorities[i] = DynArray_Create(float); }
	searchContext.NextFamilyIndex = DynArray_CreateWithSize(U32, searchContext.FamilyCount);
	for (int i = 0; i < searchContext.FamilyCount; ++i) { searchContext.NextFamilyIndex[i] = 0; }

	VulkanDevice_AssignQueue(&searchContext, Vulkan_Graphics, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0);

	if (!VulkanDevice_AssignQueue(&searchContext, Vulkan_Compute, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT) &&
	    !VulkanDevice_AssignQueue(&searchContext, Vulkan_Compute, VK_QUEUE_COMPUTE_BIT, 0)) {
		Vulkan->Queues[Vulkan_Compute].Family = Vulkan->Queues[Vulkan_Graphics].Family;
		Vulkan->Queues[Vulkan_Compute].Index  = Vulkan->Queues[Vulkan_Graphics].Index;
	}

	if (!VulkanDevice_AssignQueue(
				&searchContext, Vulkan_Transfer, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT) &&
	    !VulkanDevice_AssignQueue(&searchContext, Vulkan_Transfer, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) &&
	    !VulkanDevice_AssignQueue(&searchContext, Vulkan_Transfer, VK_QUEUE_TRANSFER_BIT, 0)) {
		Vulkan->Queues[Vulkan_Transfer].Family = Vulkan->Queues[Vulkan_Compute].Family;
		Vulkan->Queues[Vulkan_Transfer].Index  = Vulkan->Queues[Vulkan_Compute].Index;
	}

	LogD("[Vulkan] Graphics queue: %u.%u", Vulkan->Queues[Vulkan_Graphics].Family, Vulkan->Queues[Vulkan_Graphics].Index);
	LogD("[Vulkan] Compute queue: %u.%u", Vulkan->Queues[Vulkan_Compute].Family, Vulkan->Queues[Vulkan_Compute].Index);
	LogD("[Vulkan] Transfer queue: %u.%u", Vulkan->Queues[Vulkan_Transfer].Family, Vulkan->Queues[Vulkan_Transfer].Index);

	VkDeviceQueueCreateInfo* queueCIs = DynArray_CreateWithSize(VkDeviceQueueCreateInfo, Vulkan_QueueType_MAX);
	U32 familyCount                   = 0;
	U32 queueCount                    = 0;
	for (U32 i = 0; i < searchContext.FamilyCount; ++i) {
		if (searchContext.NextFamilyIndex[i] > 0) {
			VkDeviceQueueCreateInfo* ci = &queueCIs[familyCount++];
			queueCount += searchContext.NextFamilyIndex[i];

			ci->sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			ci->pNext            = NULL;
			ci->flags            = 0;
			ci->queueFamilyIndex = i;
			ci->queueCount       = searchContext.NextFamilyIndex[i];
			ci->pQueuePriorities = searchContext.FamilyPriorities[i];
		}
	}

	DynArray_Destroy(searchContext.NextFamilyIndex);
	DynArray_Destroy(searchContext.FamilyProperties);

	VkDeviceCreateInfo deviceCI = {.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	                               .pNext                   = NULL,
	                               .flags                   = 0,
	                               .queueCreateInfoCount    = familyCount,
	                               .pQueueCreateInfos       = queueCIs,
	                               .enabledLayerCount       = 0,
	                               .ppEnabledLayerNames     = NULL,
	                               .enabledExtensionCount   = DynArray_Size(Vulkan->DeviceInfo.EnabledExtensions),
	                               .ppEnabledExtensionNames = Vulkan->DeviceInfo.EnabledExtensions,
	                               .pEnabledFeatures        = NULL};

	VkAssert(
		Vulkan->vk.CreateDevice(Vulkan->Device->PhysicalDevice, &deviceCI, &Vulkan->Allocator, &Vulkan->Device->Device));

	DynArray_Destroy(queueCIs);
	for (int i = 0; i < searchContext.FamilyCount; ++i) { DynArray_Destroy(searchContext.FamilyPriorities[i]); }
	DynArray_Destroy(searchContext.FamilyPriorities);

	Vulkan_LoadDevice();

	for (int i = 0; i < Vulkan_QueueType_MAX; ++i) {
		Vulkan->vk.GetDeviceQueue(
			Vulkan->Device->Device, Vulkan->Queues[i].Family, Vulkan->Queues[i].Index, &Vulkan->Queues[i].Queue);
	}

	return TRUE;
}

B8 VulkanDevice_AssignQueue(VulkanDevice_QueueSearchContext* context,
                            Vulkan_QueueType type,
                            VkQueueFlags required,
                            VkQueueFlags ignored) {
	for (U32 q = 0; q < context->FamilyCount; ++q) {
		VkQueueFamilyProperties* family = &context->FamilyProperties[q];
		if ((family->queueFlags & required) != required) { continue; }
		if (family->queueFlags & ignored || family->queueCount == 0) { continue; }

		Vulkan->Queues[type].Family = q;
		Vulkan->Queues[type].Index  = context->NextFamilyIndex[q]++;
		--family->queueCount;
		DynArray_PushValue(context->FamilyPriorities[q], 1.0f);

		return TRUE;
	}

	return FALSE;
}
