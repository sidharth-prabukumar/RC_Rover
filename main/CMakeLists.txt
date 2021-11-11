#target_include_directories(test PRIVATE ${YOUR_DIRECTORY})
idf_component_register(
                        SRCS "src/main.c"
                        SRCS "src/motor/motor.c"
                        SRCS "src/gps/gps.c"
                    
                        INCLUDE_DIRS "include/")


