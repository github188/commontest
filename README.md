## CommonTest
This is test project, include some common test functions.

Dependency: OpenCV

Organized by CMake (> 2.8)

Sandy Yann

Aug. 4 2017

Initial version was writen during I worked at KEDACOM, Thanks KEDACOM for my first six years after graduation.

## Window cmake use OpenCV
OpenCV_DIR = "yourself git clone opencv code and build position"\build

#### For example:
OpenCV_DIR = C:\XipingYan_Code\OpenSourceCode\mygithub\opencv\build


## Add gflags thirdparty lib, modify CMakeLists.txt as follow:
include_directories(${CMAKE_BINARY_DIR}/thirdparty/gflags/include)<br>
add_subdirectory(thirdparty/gflags)<br>
target_link_libraries(${TARGET_NAME} gflags)<br>

#### Submodule
gflags will be used as submodule
Submodule usage:
1. git submodule add https://github.com/gflags/gflags.git ./gflags <tb> // add submodule
2. git submodule init	 <tb> // init submodule code folder
3. git submodule update  <tb> // get submodule code

