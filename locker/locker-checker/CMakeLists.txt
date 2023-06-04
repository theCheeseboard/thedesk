project(locker VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(SOURCES
        main.cpp
    )

add_executable(locker-checker ${SOURCES} ${HEADERS})
cntp_target_name(locker-checker "td-locker-checker")

target_link_libraries(locker-checker Qt::Core crypt)

install(TARGETS locker-checker
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        BUNDLE DESTINATION /Applications)
