project(Nara-Engine LANGUAGES C)

add_library(Nara SHARED)
target_compile_definitions(Nara PRIVATE NARA_BUILD)
target_include_directories(Nara PUBLIC Include)

add_subdirectory(Source)
