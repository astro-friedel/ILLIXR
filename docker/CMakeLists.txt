
string(TOUPPER ${MAKE_DOCKERFILE} MAKE_DOCKERFILE)
if(MAKE_DOCKERFILE MATCHES "^YES|ON")
    set(MAKE_DOCKERFILE "UBUNTU-22")
else()
    string(REPLACE "," ";" MAKE_DOCKERFILE ${MAKE_DOCKERFILE})
endif()
set(MAKE_DOCKERFILE ${MAKE_DOCKERFILE} CACHE STRING "")


set(COUNTER 0)
set(OPERATING_SYSTEMS "")
set(IN_SYSTEMS FALSE)
set(IN_OS FALSE)
set(IN_VERSIONS FALSE)
set(CURRENT_OS "")

# get the list of possible operating systems
file(READ ${CMAKE_SOURCE_DIR}/docs/docs/modules.json MODULE_TEMP)
string(REPLACE "@PROFILES@" "" MODULE_JSON "${MODULE_TEMP}")
file(WRITE ${CMAKE_BINARY_DIR}/docker/modules-comp.json "${MODULE_JSON}")

# newer versions of CMake can directly interpret JSON in strings
if(CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND CMAKE_MINOR_VERSION GREATER_EQUAL 19)
    string(JSON POSSIBLE_SYSTEMS GET ${MODULE_JSON} systems)
    string(JSON NUM_OS LENGTH ${POSSIBLE_SYSTEMS})
    math(EXPR NUM_OS "${NUM_OS} - 1")
    foreach(IDX RANGE ${NUM_OS})
        string(JSON CURRENT_OS GET ${POSSIBLE_SYSTEMS} ${IDX} name)
        list(APPEND OPERATING_SYSTEMS ${CURRENT_OS})
        set(${CURRENT_OS}_VERSIONS "")
        string(JSON VERSIONS GET ${POSSIBLE_SYSTEMS} ${IDX} versions)
        string(JSON NUM_VER LENGTH ${VERSIONS})
        math(EXPR NUM_VER "${NUM_VER} - 1")
        foreach(VIDX RANGE ${NUM_VER})
            string(JSON CVERS GET ${VERSIONS} ${VIDX})
            list(APPEND ${CURRENT_OS}_VERSIONS ${CVERS})
        endforeach()
    endforeach()
else()
    # for legacy (pre 3.19) versions of CMake, we have to parse the JSON ourselves

    # first we need to replace [, ], and \n with |, ?, and ; respectively otherwise the string will be properly
    #  split into a list
    string(REPLACE "[" "|" MODULE_JSON ${MODULE_JSON})
    string(REPLACE "]" "?" MODULE_JSON ${MODULE_JSON})
    string(REPLACE "\n" ";" MODULE_JSON ${MODULE_JSON})

    foreach(LINE IN LISTS MODULE_JSON)
        # look for the systems section
        if(NOT IN_SYSTEMS)
            string(FIND ${LINE} "systems" FOUND)
            if(${FOUND} GREATER -1)
                set(IN_SYSTEMS TRUE)
                math(EXPR COUNTER "${COUNTER} + 1")
            endif()
        else()
            # keep track of the number of open and close brackets (replaced by | and ?)
            string(FIND ${LINE} "|" FOUND)
            if(FOUND GREATER -1)
                math(EXPR COUNTER "${COUNTER} + 1")
            endif()
            string(FIND ${LINE} "?" FOUND)
            if(FOUND GREATER -1)
                math(EXPR COUNTER "${COUNTER} - 1")
                if(IN_VERSIONS)
                    set(IN_VERSIONS FALSE)
                endif()
            endif()
            # if the systems section is closed, then exit as we are done
            if(${COUNTER} EQUAL 0)
                break()
            endif(${COUNTER} EQUAL 0)
            if(NOT IN_OS)
                # look for a new operating system name
                string(FIND ${LINE} "name" FOUND)
                if(${FOUND} GREATER -1)
                    # parse the name of the operating system
                    string(REPLACE "\"name\": " "" LINE ${LINE})
                    string(REPLACE "\"" "" LINE ${LINE})
                    string(REPLACE "," "" LINE ${LINE})
                    string(STRIP ${LINE} LINE)
                    set(CURRENT_OS ${LINE})
                    list(APPEND OPERATING_SYSTEMS "${CURRENT_OS}")
                    set(${CURRENT_OS}_VERSIONS "")
                    set(IN_OS TRUE)
                    continue()
                endif(${FOUND} GREATER -1)
            else()
                # look for the closing of the operating system section
                string(FIND ${LINE} "}" FOUND)
                if(${FOUND} GREATER -1)
                    set(IN_OS FALSE)
                    continue()
                endif(${FOUND} GREATER -1)
                if(NOT IN_VERSIONS)
                    # look for versions section
                    string(FIND ${LINE} "versions" FOUND)
                    if(${FOUND} GREATER -1)
                        set(IN_VERSIONS TRUE)
                        continue()
                    endif()
                else()
                    # get the versions for this OS
                    string(REGEX MATCH "([0-9]+)" VERSION ${LINE})
                    if(VERSION)
                        list(APPEND ${CURRENT_OS}_VERSIONS ${VERSION})
                        #list(APPEND OPERATING_SYSTEMS "${CURRENT_OS}${VERSION}")
                    endif(VERSION)
                endif(NOT IN_VERSIONS)
            endif(NOT IN_OS)
        endif(NOT IN_SYSTEMS)
    endforeach()
endif(CMAKE_MAJOR_VERSION GREATER_EQUAL 3 AND CMAKE_MINOR_VERSION GREATER_EQUAL 19)

# copy the python files which deal with the dockerfile generation to the build directory
configure_file(${CMAKE_SOURCE_DIR}/docker/generate_dockerfiles.py
               ${CMAKE_BINARY_DIR}/docker/generate_dockerfiles.py
               COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/docker/images.py
               ${CMAKE_BINARY_DIR}/docker/images.py
               COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/docker/build.sh
               ${CMAKE_BINARY_DIR}/docker/build.sh
               COPYONLY)

file(CHMOD ${CMAKE_BINARY_DIR}/docker/generate_dockerfiles.py ${CMAKE_BINARY_DIR}/docker/build.sh
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ)
file(CHMOD ${CMAKE_BINARY_DIR}/docker/images.py
     PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# create the dockerfile generation target
add_custom_target(dockerfile ALL
                  COMMAND ${CMAKE_BINARY_DIR}/docker/generate_dockerfiles.py -f modules-comp.json --os=${MAKE_DOCKERFILE})

if(BUILD_DOCKER)
    # copy the python file which builds the docker files to the build directory
    configure_file(${CMAKE_SOURCE_DIR}/docker/build_docker.py
                   ${CMAKE_BINARY_DIR}/docker/build_docker.py
                   COPYONLY)
    file(CHMOD ${CMAKE_BINARY_DIR}/docker/build_docker.py
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_READ)
    # add a target that will build all specified docker images
    add_custom_target(docker ALL
                      COMMAND ${CMAKE_BINARY_DIR}/docker/build_docker.py --os=${MAKE_DOCKERFILE}
                      DEPENDS dockerfile)
    # generate targets for all of the individual docker images
    if(MAKE_DOCKERFILE STREQUAL "ALL")
        foreach(OSV IN LISTS OPERATING_SYSTEMS)
            set(CUR_VERS "")
            add_custom_target(docker_${OSV}
                              COMMAND ${CMAKE_BINARY_DIR}/docker/build_docker.py --os=${OSV}
                              DEPENDS dockerfile)
            foreach(VERSION IN LISTS ${OSV}_VERSIONS)
                add_custom_target(docker_${OSV}-${VERSION}
                                  COMMAND ${CMAKE_BINARY_DIR}/docker/build_docker.py --os=${OSV}-${VERSION}
                                  DEPENDS dockerfile)
            endforeach ()
        endforeach ()
    else()
        # generate targets for each of the specified docker images
        foreach(ITEM IN LISTS MAKE_DOCKERFILE)
            string(FIND ${ITEM} "-" FOUND)
            set(MTCH FALSE)
            if(FOUND GREATER -1)
                string(REPLACE "-" ";" VER_STR ${ITEM})
                list(GET VER_STR 0 VERSION)
                string(TOUPPER ${VERSION} VERSION)
                list(GET VER_STR 1 VER_NUM)
                foreach(OPS IN LISTS OPERATING_SYSTEMS)
                    string(TOUPPER ${OPS} UPPER_OPS)
                    if(VERSION STREQUAL UPPER_OPS)
                        list(FIND ${OPS}_VERSIONS ${VER_NUM} FOUND)
                        if(FOUND GREATER -1)
                            set(MTCH TRUE)

                            add_custom_target(docker_${OPS}-${VER_NUM}
                                              COMMAND ${CMAKE_BINARY_DIR}/docker/generate_dockerfiles.py --os=${OPS}-${VER_NUM}
                                              DEPENDS dockerfile)
                            break()
                        endif(FOUND GREATER -1)
                    endif(VERSION STREQUAL UPPER_OPS)
                endforeach()
            else()
                string(TOUPPER ${ITEM} UPPER_OS)
                foreach(OPS IN LIST OPERATING_SYSTEMS)
                    string(TOUPPER ${OPS} UPPER_OPS)
                    if(UPPER_OS STREQUAL UPPER_OPS)
                        set(MTCH TRUE)
                        foreach(VER IN LISTS ${OPS}_VERSIONS)
                            add_custom_target(docker_${OPS}-${VER}
                                              COMMAND ${CMAKE_BINARY_DIR}/docker/generate_dockerfiles.py --os=${OPS}-${VER}
                                              DEPENDS dockerfile)
                        endforeach ()
                        break()
                    endif(UPPER_OS STREQUAL UPPER_OPS)
                endforeach()
            endif(FOUND GREATER -1)
            if(NOT MTCH)
                message(FATAL_ERROR "Could not find ${ITEM}")
            endif()
        endforeach()
    endif(MAKE_DOCKERFILE STREQUAL "ALL")
endif(BUILD_DOCKER)
