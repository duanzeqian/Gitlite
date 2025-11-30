cmake_minimum_required(VERSION 3.5)

project (gitlite)

if(DEFINED ENV{ONLINE_JUDGE})
    add_definitions(-DONLINE_JUDGE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 移除自定义的输出路径设置，使用默认的构建目录

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Repository.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GitliteException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GitliteObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Blob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Tree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Add.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/CommitCmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Rm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/GlobalLog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Find.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Checkout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Commands/Status.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SomeObj.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GitliteException.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Repository.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GitliteObject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Blob.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Tree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Init.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Add.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/CommitCmd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Rm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/GlobalLog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Find.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Checkout.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/Status.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SomeObj.h
)

add_executable(gitlite ${SOURCES})

# 设置 C++ 标准
set_target_properties(gitlite PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(gitlite
  PRIVATE
    -g
)
