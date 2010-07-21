SET(QNXNTO 1)

# this one is important
SET(CMAKE_SYSTEM_NAME QNX)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 6.4.1)

SET(CMAKE_EXE_LINKER_FLAGS "-V4.4.2,gcc_ntox86_gpp  -L/home/docent/devel/qnx_target/mrlib/lib")
SET(CMAKE_SHARED_LINKER_FLAGS "-V4.4.2,gcc_ntox86_gpp -L/home/docent/devel/qnx_target/mrlib/lib")
SET(CMAKE_MODULE_LINKER_FLAGS "-V4.4.2,gcc_ntox86_gpp  -L/home/docent/devel/qnx_target/mrlib/lib")

# specify the cross compiler
if(CMAKE_HOST_WIN32)
SET(CMAKE_C_COMPILER   "$ENV{QNX_HOST}/usr/bin/qcc.exe")
SET(CMAKE_CXX_COMPILER "$ENV{QNX_HOST}/usr/bin/QCC.exe")
else(CMAKE_HOST_WIN32)
SET(CMAKE_C_COMPILER   "$ENV{QNX_HOST}/usr/bin/qcc")
SET(CMAKE_CXX_COMPILER "$ENV{QNX_HOST}/usr/bin/QCC")
endif(CMAKE_HOST_WIN32)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH $ENV{QNX_TARGET})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(_CMAKE_TOOLCHAIN_PREFIX "ntox86-")
SET(_CMAKE_TOOLCHAIN_LOCATION "$ENV{QNX_HOST}/usr/bin")

# warn about almost everything except:
# -Wcast-qual
# -Wshadow
# -Wsign-compare
# optimization option -O1 is required for the compiler to find uninitialized local variables.
set(CMAKE_CXX_FLAGS "-w9 -Wpointer-arith -Wwrite-strings -Wno-sign-compare -O1")

## GCC is the default compiler on QNX 6.3.
#INCLUDE(Platform/gcc)
#
## The QNX GCC does not seem to have -isystem so remove the flag.
#SET(CMAKE_INCLUDE_SYSTEM_FLAG_C)
#SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX)
#
#SET(CMAKE_DL_LIBS "")
#SET(CMAKE_SHARED_LIBRARY_C_FLAGS "")
#SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")
#SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-shared")
#SET(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
#SET(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP ":")
#SET(CMAKE_SHARED_LIBRARY_RPATH_LINK_C_FLAG "-Wl,-rpath-link,")
#SET(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-Wl,-soname,")
#SET(CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG "-Wl,-soname,")
#SET(CMAKE_EXE_EXPORTS_C_FLAG "-Wl,--export-dynamic")
#SET(CMAKE_EXE_EXPORTS_CXX_FLAG "-Wl,--export-dynamic")
#
## Shared libraries with no builtin soname may not be linked safely by
## specifying the file path.
#SET(CMAKE_PLATFORM_USES_PATH_WHEN_NO_SONAME 1)
#
## Initialize C link type selection flags.  These flags are used when
## building a shared library, shared module, or executable that links
## to other libraries to select whether to use the static or shared
## versions of the libraries.
#FOREACH(type SHARED_LIBRARY SHARED_MODULE EXE)
#  SET(CMAKE_${type}_LINK_STATIC_C_FLAGS "-Wl,-Bstatic")
#  SET(CMAKE_${type}_LINK_DYNAMIC_C_FLAGS "-Wl,-Bdynamic")
#ENDFOREACH(type)
## force the language to be c++ since qnx only has gcc and not g++ and c++?
#SET(CMAKE_CXX_COMPILE_OBJECT
#  "<CMAKE_CXX_COMPILER> <DEFINES> <FLAGS> -o <OBJECT> -c <SOURCE>")
#
#INCLUDE(Platform/UnixPaths)
