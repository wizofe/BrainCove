FIND_PACKAGE(Qt4 REQUIRED) # find and setup Qt4 for this project
FIND_PACKAGE(VTK REQUIRED) # find and setup VTK for this project
IF (USE_NQVTK)
	FIND_PACKAGE( NQVTK REQUIRED ) # find and setup NQVTK for this project
	FIND_PACKAGE( NQVTKGui REQUIRED ) # find and setup NQVTK for this project
	INCLUDE( ${NQVTK_USE_FILE} )
	INCLUDE( ${NQVTKGui_USE_FILE} )
ENDIF (USE_NQVTK)

IF ( QT4_FOUND )
  SET( QT_USE_QTMAIN TRUE )
  SET( QT_USE_QTOPENGL TRUE )
  SET( QT_USE_QTXML TRUE )
  INCLUDE( ${QT_USE_FILE} )

IF(WIN32)
  SET(CMAKE_DEBUG_POSTFIX "d")
  FIND_PACKAGE( GLEW REQUIRED ) #we need glew on Windows for glTexture3D...
ENDIF(WIN32)
  
ENDIF( QT4_FOUND )

IF ( VTK_FOUND )
  INCLUDE( ${USE_VTK_FILE} )
ENDIF( VTK_FOUND )

IF( GLEW_FOUND )
  INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
ENDIF( GLEW_FOUND )

#IF( OPENCL_FOUND )
  INCLUDE_DIRECTORIES( ${_OPENCL_CPP_INCLUDE_DIRS} )
#ENDIF( OPENCL_FOUND )

set(matrixiotest_srcs
	main.cpp
	mainwindow.cpp
) 

set(matrixiotest_hdrs
	mainwindow.h
)

set(matrixiotest_uis
	mainwindow.ui
)

set(matrixiotest_rcc 
	mainwindow.qrc
)

set(matrixio_lib
	${CMAKE_CURRENT_BINARY_DIR}/../MatrixIO
)

QT4_WRAP_CPP(matrixiotest_moc ${matrixiotest_hdrs})

QT4_WRAP_UI(matrixiotest_uis_h ${matrixiotest_uis})

QT4_AUTOMOC( ${matrixiotest_srcs} )

INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/Shared
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/MatrixIO
  ${CMAKE_SOURCE_DIR}/Rendering
  ${CMAKE_SOURCE_DIR}/UI
  ${CMAKE_BINARY_DIR}/UI  
)

ADD_EXECUTABLE(MatrixIOTests ${matrixiotest_srcs} ${matrixiotest_moc} ${matrixiotest_hdrs} ${matrixiotest_uis_h} ${matrixiotest_rcc_src})

TARGET_LINK_LIBRARIES (
  MatrixIOTests
  "MatrixIO"
  "Rendering"
  "UI"
  ${QT_LIBRARIES}
  ${VTK_LIBS}
)

IF(USE_NQVTK)
    ADD_DEFINITIONS(-DUSE_NQVTK)

	TARGET_LINK_LIBRARIES (
		MatrixIOTests
		  ${NQVTK_LIBRARIES}
		  ${NQVTKGui_LIBRARIES}  
	)
ENDIF(USE_NQVTK)
