
FIND_PACKAGE(KDE4)
FIND_PACKAGE(Qt4 REQUIRED)

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTDBUS TRUE)

SET(CMAKE_CXX_FLAGS "-D USE_KDE")

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# Project files
SET( mysmsSources 
  kdenotification.cpp
	jsinterface.cpp
	main.cpp
	mainwindow.cpp
	networkcookiejar.cpp
  qtlocalpeer.cpp
	qtsingleapplication.cpp
  )

# UI files
KDE4_ADD_UI_FILES( mysmsSources 
   )

# Resources
QT4_ADD_RESOURCES( mysmsResources mysms.qrc )

KDE4_ADD_EXECUTABLE( mysms ${mysmsSources} ${mysmsResources} )

TARGET_LINK_LIBRARIES( mysms 
	${QT_LIBRARIES}
	${KDE4_KDEUI_LIBS} ) 

INSTALL( TARGETS mysms DESTINATION ${KDE4_BIN_INSTALL_DIR} )
INSTALL( FILES mysms.notifyrc DESTINATION ${DATA_INSTALL_DIR}/mysms)
