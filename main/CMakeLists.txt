#target_include_directories(test PRIVATE ${YOUR_DIRECTORY})
idf_component_register(SRCS "src/main.cpp"
                            "src/motor/motor.cpp"
                            "src/gps/gps.cpp"
                        INCLUDE_DIRS "include/")


