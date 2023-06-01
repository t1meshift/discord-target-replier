set(PROJECT_NAME "discord_target_replier")

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
	set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "The vcpkg triplet")
endif()

cmake_minimum_required(VERSION 3.22)
project("${PROJECT_NAME}"
	VERSION 1.0.0
	DESCRIPTION "A simple Discord bot for replying messages from a given user."
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable("${PROJECT_NAME}" main.cpp Config.cpp Config.h)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE nlohmann_json::nlohmann_json)

find_package(dpp CONFIG REQUIRED)
target_link_libraries("${PROJECT_NAME}" PRIVATE dpp::dpp)

add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${PROJECT_SOURCE_DIR}/config.example.json"
	$<TARGET_FILE_DIR:${PROJECT_NAME}>)

# I can't just install RUNTIME_DEPENDENCY_SET as it includes
# a bunch of system libraries. This build system is a joke.
# https://gitlab.kitware.com/cmake/cmake/-/issues/22006
set(INCLUDE_LIBS
	"dpp(.+)"
	"libcrypto(.+)"
	"libssl(.+)"
	"libsodium(.+)"
	"opus(.+)"
	"zlib(.+)"
	)

include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}"
	RUNTIME_DEPENDENCIES
	PRE_INCLUDE_REGEXES ${INCLUDE_LIBS}
	PRE_EXCLUDE_REGEXES ".+"
	CONFIGURATIONS Release
	DESTINATION ".")
install(FILES "${PROJECT_SOURCE_DIR}/config.example.json"
	CONFIGURATIONS Release
	DESTINATION ".")

set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)

set(CPACK_SOURCE_IGNORE_FILES .git/ .github/ .vscode/ .gitignore
	${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/
	cmake-build-debug/ cmake-build-release/ ) # Hello, CLion

include(CPack)