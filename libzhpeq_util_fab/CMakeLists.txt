add_compile_options(-fPIC)
add_library(zhpeq_util_fab STATIC libzhpeq_util_fab.c)
target_link_libraries(zhpeq_util_fab PRIVATE zhpeq_util PUBLIC fabric)

#install(TARGETS zhpeq_util_fab DESTINATION lib)
#install(FILES ${CMAKE_SOURCE_DIR}/include/zhpeq_util_fab.h
#  DESTINATION include)

