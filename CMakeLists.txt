idf_component_register(SRC_DIRS  src driver
                        INCLUDE_DIRS . inc 
                        REQUIRES  idf-i2c
                      )

add_prebuilt_library(algobsec "./lib/libalgobsec.a")
target_link_libraries(${COMPONENT_LIB} PRIVATE algobsec)