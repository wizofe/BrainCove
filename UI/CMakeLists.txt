cmake_minimum_required(VERSION 2.8)

SET( ProjectName UI )
PROJECT( ${ProjectName} )

FIND_PACKAGE(Qt4 REQUIRED) # find and setup Qt4 for this project
FIND_PACKAGE(VTK REQUIRED) # find and setup VTK for this project
FIND_PACKAGE(OpenCL REQUIRED ) # find and setup OpenCL for this project
FIND_PACKAGE(GLEW REQUIRED)

IF( GLEW_FOUND )
  INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
ENDIF( GLEW_FOUND )


IF ( QT4_FOUND )
  SET( QT_USE_QTMAIN TRUE )
  SET( QT_USE_QTOPENGL TRUE )
  SET( QT_USE_QTXML TRUE )
  INCLUDE( ${QT_USE_FILE} )

ENDIF( QT4_FOUND )

#IF ( VTK_FOUND )
  INCLUDE( ${VTK_USE_FILE} )
#ENDIF( VTK_FOUND )

IF( OPENCL_FOUND )
  INCLUDE_DIRECTORIES( ${_OPENCL_CPP_INCLUDE_DIRS} )
ENDIF( OPENCL_FOUND )

set(ui_srcs 
	#mainwindow.cpp
)

set(ui_hdrs
	dlldefines.h
	#mainwindow.h
)

set(ui_uis
	#mainwindow.ui
)

set(ui_qrc
	#mainwindow.qrc
)

QT4_WRAP_CPP(ui_moc ${ui_hdrs})
QT4_WRAP_UI(ui_uis_h ${ui_uis})
QT4_AUTOMOC( ${ui_srcs} )
QT4_ADD_RESOURCES(ui_rcc_src ${ui_qrc})

include("${CMAKE_CURRENT_SOURCE_DIR}/editors/editors.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/3rdparty.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/views/views.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/styles/styles.cmake")

set(project_srcs
	${ui_srcs}
	${editors_srcs}
	${editors_moc}
	${3rdparty_srcs}
	${views_srcs}
	${views_moc}
	${styles_srcs}
	${styles_moc}
)

set(project_hdrs
	${ui_hdrs}
	${editors_hdrs}
	${3rdparty_hdrs}
	${views_hdrs}
	${styles_hdrs}
)

set(project_uis
	${ui_uis}
	${editors_uis}
	${3rdparty_uis}
	${views_uis}
	${styles_uis}
)

set(project_rcc
	ui.qrc
)

QT4_ADD_RESOURCES(project_rcc_src ${project_rcc})

SOURCE_GROUP( Viewers FILES ${views_srcs} ${views_hdrs} ${views_uis} )
SOURCE_GROUP( Editors FILES ${editors_srcs} ${editors_hdrs} ${editors_uis} )
SOURCE_GROUP( ThirdParty FILES ${3rdparty_srcs} ${3rdparty_hdrs} ${3rdparty_uis} )
SOURCE_GROUP( Styles FILES ${styles_srcs} ${styles_hdrs} ${styles_uis} )

add_library(${ProjectName} SHARED ${project_srcs} ${project_hdrs} ${project_uis} ${project_rcc_src})

message(${CMAKE_CURRENT_BINARY_DIR})

INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/Shared
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/MatrixIO
  ${CMAKE_SOURCE_DIR}/Rendering
  ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_LINK_LIBRARIES(${ProjectName}
    ${VTK_LIBS}	
	vtkHybrid
	vtkVolumeRendering
	QVTK
	${QT_LIBRARIES}
	${OPENCL_LIBRARIES}
	Rendering
)
