#-- Add an Option to toggle the generation of the API documentation

OPTION(BUILD_DOCUMENTATION "HTML API Dokumentation mit Doxygen erstellen" OFF)
IF(BUILD_DOCUMENTATION)

  FIND_PACKAGE(Doxygen)
  IF (NOT DOXYGEN_FOUND)
    MESSAGE("Doxygen wird für die Dokumentation benötigt. Bitte installiere es.")
  ELSE()

    #-- Configure the Template Doxyfile for our specific project
    CONFIGURE_FILE(${CMAKE_MODULE_PATH}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)

    #-- Add a custom target to run Doxygen when ever the project is built
    ADD_CUSTOM_TARGET (Docs ALL
    	COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    	SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
    
    # IF you do NOT want the documentation to be generated EVERY time you build the project
    # then leave out the 'ALL' keyword from the above command.
  ENDIF()
ENDIF()