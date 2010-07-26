file(GLOB ATI_SRCFILE *.cc)
add_library(ati6284MS ${ATI_SRCFILE})
target_link_libraries(ati6284MS protobuf kiper)
