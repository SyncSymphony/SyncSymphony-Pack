cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "main.c"
		       SRCS "ntp.c"
		       SRCS "websocket.c"
		       SRCS "metronome.c"
                    INCLUDE_DIRS "")
