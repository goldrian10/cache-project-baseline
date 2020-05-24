# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
# Link Cache Cachetest GTest and pthread library
include_directories(${CacheSim_SOURCE_DIR}/include)

# Add source files to executable
add_executable(cachetest test_main.cpp )
target_sources(cachetest PUBLIC ${CacheSim_SOURCE_DIR}/src/debug_utilities.cpp
				${CacheSim_SOURCE_DIR}/src/L1cache.cpp
				${CacheSim_SOURCE_DIR}/src/L2cache.cpp
				${CacheSim_SOURCE_DIR}/src/Victimcache.cpp
                                ${CacheSim_SOURCE_DIR}/src/L1Obl.cpp
				${CacheSim_SOURCE_DIR}/test/L1cache_test.cpp
				${CacheSim_SOURCE_DIR}/test/L2cache_test.cpp
				${CacheSim_SOURCE_DIR}/test/L1Obl_test.cpp
				${CacheSim_SOURCE_DIR}/test/Victimcache_test.cpp)

target_link_libraries(cachetest ${GTEST_LIBRARIES} pthread)
