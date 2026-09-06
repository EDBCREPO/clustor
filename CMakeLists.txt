cmake_minimum_required(VERSION 3.6)
project( AMG_SERVER CXX) 

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

include(FetchContent)
FetchContent_Declare(
	nodepp-apify
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-apify
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-apify)

include(FetchContent)
FetchContent_Declare(
	nodepp-express
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-express
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-express)

set(EMCC_LINK_FLAGS
	"-O3"
	"-march=native"
	"-mtune=native"
)

add_executable( main ./main.cpp )
target_link_options  ( main PRIVATE ${EMCC_LINK_FLAGS})
target_link_libraries( main PUBLIC nodepp nodepp-apify nodepp-express )