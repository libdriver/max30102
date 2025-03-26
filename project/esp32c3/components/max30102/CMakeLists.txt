set(src_files 
    "../../../../src/driver_max30102.c"
    "interface/driver_max30102_interface_esp32.c"
    "../../../../test/driver_max30102_fifo_test.c"
    "../../../../test/driver_max30102_register_test.c"
    "../../../../example/driver_max30102_fifo.c"
    
    )

set(include_files 
    "../../../../src"
    "../../../../interface"
    "../../../../test"
    "../../../../example"
)

idf_component_register(SRCS ${src_files}
                    INCLUDE_DIRS ${include_files}
                    REQUIRES driver
                    )