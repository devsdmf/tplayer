######## targets ##########

SET(tplayer_SRCS
    main.cpp
)

INCLUDE_DIRECTORIES(${MIKMOD_INCLUDE_DIR})

ADD_EXECUTABLE(tplayer ${tplayer_SRCS})

TARGET_LINK_LIBRARIES(tplayer ${MIKMOD_LIBRARIES} ${EXTRA_LIBS})

INSTALL(TARGETS tplayer DESTINATION bin)

