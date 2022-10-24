project(Nara-Engine LANGUAGES C)

include(FindVulkan)

add_library(Nara SHARED)
target_compile_definitions(Nara PRIVATE NARA_BUILD)
target_include_directories(Nara PUBLIC Include)
target_link_libraries(Nara PRIVATE Vulkan::Vulkan)

add_subdirectory(Source)
