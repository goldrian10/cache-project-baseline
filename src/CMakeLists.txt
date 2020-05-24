include_directories(${CacheSim_SOURCE_DIR}/include)
add_executable(cache main.cpp)
target_sources(cache PUBLIC debug_utilities L1cache L2cache Victimcache L1Obl)
