
@echo
:: 工程目录
set WORKSPACE=%~dp0\..
set BUILD_DIR=%WORKSPACE%\build
set CMAKE_DIR=%BUILD_DIR%\cmake

if exist %BUILD_DIR% (	
	rmdir /S /Q %BUILD_DIR%
)
mkdir %BUILD_DIR%
mkdir %CMAKE_DIR%

cd %CMAKE_DIR%
cmake  -DCMAKE_INSTALL_PREFIX=%WORKSPACE% ../../src/lib_cpp_common

cmake --build .