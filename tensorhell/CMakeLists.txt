cmake_minimum_required(VERSION 2.8)

project(tensorheaven_test)

find_path(EIGEN_INCLUDE_DIR "eigen" PATHS $ENV{EIGEN_INCLUDE_DIR} NO_DEFAULT_PATH)
find_path(BOOST_INCLUDE_DIR "boost" PATHS $ENV{BOOST_INCLUDE_DIR} NO_DEFAULT_PATH)

OPTION(ASM_EXAM "Keep ASM intermediaries for inspection of compiler optimizations." OFF)
OPTION(NO_OPTIMIZE "Compile without optimization (useful for debugging; not implemented for MSVC currently)." OFF)

message("CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  message("compiler is MSVC")
  set(MSVC 1)
  set(CLANG 0)
  set(GCC 0)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("compiler is Clang")
  set(MSVC 0)
  set(CLANG 1)
  set(GCC 0)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GCC")
  message("compiler is GCC")
  set(MSVC 0)
  set(CLANG 0)
  set(GCC 1)
else()
  message(FATAL ERROR "unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

if(BUILD_WINDOWS)
  message("BUILD_WINDOWS is enabled")
endif()
if(BUILD_MAC)
  message("BUILD_MAC is enabled")
endif()
if(BUILD_LINUX)
  message("BUILD_LINUX is enabled")
endif()

# the root directory of the project (the one containing the tenh/ dir) is the only necessary include dir
include_directories(${tensorheaven_test_SOURCE_DIR}/include ${EIGEN_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})

if(MSVC)
  message("setting options for MSVC")
  set(BASE_MSVC_FLAGS /arch:SSE2 /bigobj)
  # misnomer: The above MSVC flags are not used in Debug mode
  set(PRODUCTION_MSVC_FLAGS /GL /Ox /Oi /Ot /Ob2 )
  set(COMPILER_DEBUG_FLAGS /Zi /D_DEBUG /MDd /Od /fp:except /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS=1 /RTC1 /MP)
  set(COMPILER_RELEASE_FLAGS /Zi /D NDEBUG /D _CRT_SECURE_NO_WARNINGS /MD /O2 /fp:fast /MP) #  /WX (warnings as errors)
  if(NO_OPTIMIZE)
    # TODO: implement NO_OPTIMIZE for MSVC
  endif()
  # USE_PRODUCTION_FLAGS will replace /Od or /O2 with /Ox /Ob2 /Oi /Ot, adds ~20% compile time
  set(COMPILER_NOPRODUCTION_FLAGS /Zi )
  set(BASE_LINKER_FLAGS /NODEFAULTLIB:LIBCMT /ignore:4099 /DEBUG )
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF,ICF")
  set(NOPRODUCTION_LINKER_FLAGS "")
  set(PRODUCTION_LINKER_FLAGS /LTCG )
  add_definitions(-DNOMINMAX)
else(CLANG OR GCC) # clang or gcc
  message("setting options for Clang or GCC")
  set(BASE_GCC_FLAGS -std=c++11 -Wall -Werror -Wno-unused-local-typedefs -Wno-attributes ) #-msse2 -Wno-reorder -Wno-strict-aliasing -Wno-missing-braces -Wno-uninitialized -Wno-sequence-point )
  set(PRODUCTION_GCC_FLAGS "")
  set(BASE_CLANG_FLAGS -std=c++11 -Wall -Werror -Wno-local-type-template-args -Wno-tautological-compare ) #-stdlib=libc++ -Wall -Wno-deprecated-declarations -Wno-address-of-temporary -Wno-dynamic-class-memaccess -Wno-overloaded-virtual -Wno-missing-braces -Wno-unneeded-internal-declaration -Wno-reorder -Wno-local-type-template-args )
  #if(BUILD_MAC)
    set(BASE_CLANG_FLAGS ${BASE_CLANG_FLAGS} -stdlib=libc++)
  #endif()
  set(COMPILER_DEBUG_FLAGS -g3 )
  set(COMPILER_DEBUG_NO_OPTIMIZE_FLAGS -O0 -g3 )
  set(COMPILER_RELEASE_FLAGS -O3 -g0 -DNDEBUG )
  set(COMPILER_NOPRODUCTION_FLAGS "")
  set(BASE_LINKER_FLAGS "")
  set(NOPRODUCTION_LINKER_FLAGS "")
  set(PRODUCTION_LINKER_FLAGS "")
  if(APPLE)
    set(COMPILER_DEBUG_FLAGS ${COMPILER_DEBUG_FLAGS} -mmacosx-version-min=10.4 -fvisibility=hidden)
    set(COMPILER_RELEASE_FLAGS ${COMPILER_RELEASE_FLAGS} -mmacosx-version-min=10.4 -fvisibility=hidden)
    set(STRIPPED_EXE_LINKER_FLAGS "-Xlinker -unexported_symbol -Xlinker \"*\" -Xlinker -dead_strip -Xlinker -dead_strip_dylibs")
  else()
    set(STRIPPED_EXE_LINKER_FLAGS " ")
    set(COMPILER_DEBUG_FLAGS ${COMPILER_DEBUG_FLAGS} -fvisibility=hidden)
    set(COMPILER_RELEASE_FLAGS ${COMPILER_RELEASE_FLAGS} -fvisibility=hidden)
  endif()
  if(ASM_EXAM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps") # this uses -O0, which avoids inlined code being optimized out
  endif()
  if(NO_OPTIMIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0") # this uses -O0, which avoids inlined code being optimized out
  endif()
endif()

if(MSVC)
  set(COMPILER_FLAGS ${COMPILER_FLAGS} ${BASE_MSVC_FLAGS})
elseif(CLANG)
  set(COMPILER_FLAGS ${COMPILER_FLAGS} ${BASE_CLANG_FLAGS})
else(GCC)
  set(COMPILER_FLAGS ${COMPILER_FLAGS} ${BASE_GCC_FLAGS})
endif()

foreach(arg ${COMPILER_FLAGS})
  set(TEMP_COMPILER_FLAG_LIST "${TEMP_COMPILER_FLAG_LIST} ${arg}")
endforeach(arg ${COMPILER_FLAGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TEMP_COMPILER_FLAG_LIST}")

foreach(arg ${COMPILER_DEBUG_FLAGS})
  set(TEMP_COMPILER_DEBUG_FLAG_LIST "${TEMP_COMPILER_DEBUG_FLAG_LIST} ${arg}")
endforeach(arg ${COMPILER_DEBUG_FLAGS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${TEMP_COMPILER_DEBUG_FLAG_LIST}")
message("CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")

foreach(arg ${COMPILER_RELEASE_FLAGS})
  set(TEMP_COMPILER_RELEASE_FLAG_LIST "${TEMP_COMPILER_RELEASE_FLAG_LIST} ${arg}")
endforeach(arg ${COMPILER_DEBUG_FLAGS})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${TEMP_COMPILER_RELEASE_FLAG_LIST}")
message("CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")


# temp tests
# add_executable(algebraic_expression_prototype algebraic_expression_prototype.cpp)
# add_executable(asm_exam asm_exam.cpp asm_exam_separate_functions.cpp)
add_executable(c++11_usage_prototype c++11_usage_prototype.cpp)
add_executable(compile_time_generated_lookup_table compile_time_generated_lookup_table.cpp)
add_executable(conceptual_inheritance_prototype conceptual_inheritance_prototype.cpp)
add_executable(prettyprint prettyprint.cpp)
add_executable(sandbox sandbox.cpp)
add_executable(taylor_polynomial taylor_polynomial.cpp)

#set_source_files_properties(c++11_usage_prototype.cpp PROPERTIES COMPILE_FLAGS -std=c++11)
#set_source_files_properties(compile_time_generated_lookup_table.cpp PROPERTIES COMPILE_FLAGS -std=c++11)
#set_source_files_properties(sandbox.cpp PROPERTIES COMPILE_FLAGS -std=c++11)

set(adhoc_test_SRCS
    adhoc_test.cpp
    adhoc_test.hpp
    adhoc_test_00.cpp
    adhoc_test_01.cpp
    adhoc_test_02.cpp
    adhoc_test_03.cpp
    adhoc_test_04.cpp
    adhoc_test_05.cpp
    adhoc_test_06.cpp
    adhoc_test_07.cpp
    adhoc_test_08.cpp
    adhoc_test_09.cpp
    adhoc_test_10.cpp
    adhoc_test_11.cpp
    adhoc_test_12.cpp
    adhoc_test_13.cpp
    adhoc_test_14.cpp
    adhoc_test_15.cpp)
add_executable(adhoc_test ${adhoc_test_SRCS})

# test-framework-using tests
set(test_SRCS
    lvd/lvd_callstack.cpp
    lvd/lvd_callstack.hpp
    lvd/lvd_commandlineparser.cpp
    lvd/lvd_commandlineparser.hpp
    lvd/lvd_spawner.cpp
    lvd/lvd_spawner.hpp
    lvd/lvd_testsystem.cpp
    lvd/lvd_testsystem.hpp
    standard/randomize.cpp
    standard/randomize.hpp
    standard/test_abstractindex.cpp
    standard/test_abstractindex.hpp
    standard/test_array.cpp
    standard/test_array.hpp
    standard/test_basic_operator0.cpp
    standard/test_basic_operator1.cpp
    standard/test_basic_operator2.cpp
    standard/test_basic_operator3.cpp
    standard/test_basic_operator4.cpp
    standard/test_basic_operator5.cpp
    standard/test_basic_operator.hpp
    standard/test_basic_vector0.cpp
    standard/test_basic_vector1.cpp
    standard/test_basic_vector2.cpp
    standard/test_basic_vector3.cpp
    standard/test_basic_vector4.cpp
    standard/test_basic_vector5.cpp
    standard/test_basic_vector.hpp
    standard/test_dimindex.cpp
    standard/test_dimindex.hpp
    standard/test_expressiontemplate_reindex.cpp
    standard/test_expressiontemplate_reindex.hpp
    standard/test_homogeneouspolynomials0.cpp
    standard/test_homogeneouspolynomials1.cpp
    standard/test_homogeneouspolynomials2.cpp
    standard/test_homogeneouspolynomials3.cpp
    standard/test_homogeneouspolynomials4.cpp
    standard/test_homogeneouspolynomials5.cpp
    standard/test_homogeneouspolynomials.hpp
    standard/test_linearembedding0.cpp
    standard/test_linearembedding1.cpp
    standard/test_linearembedding2.cpp
    standard/test_linearembedding3.cpp
    standard/test_linearembedding4.cpp
    standard/test_linearembedding5.cpp
    standard/test_linearembedding.hpp
    standard/test_multivariatepolynomials0.cpp
    standard/test_multivariatepolynomials1.cpp
    standard/test_multivariatepolynomials2.cpp
    standard/test_multivariatepolynomials3.cpp
    standard/test_multivariatepolynomials4.cpp
    standard/test_multivariatepolynomials5.cpp
    standard/test_multivariatepolynomials.hpp
    standard/test_split_and_bundle.cpp
    standard/test_split_and_bundle.hpp
    standard/test_tuple.cpp
    standard/test_tuple.hpp
    standard/test_typle.cpp
    standard/test_typle.hpp)
add_executable(test standard/test.cpp ${test_SRCS})
include_directories(${tensorheaven_test_SOURCE_DIR}/../include
                    ${tensorheaven_test_SOURCE_DIR}/lvd
                    ${tensorheaven_test_SOURCE_DIR}/standard)

add_subdirectory(compile_tests)
