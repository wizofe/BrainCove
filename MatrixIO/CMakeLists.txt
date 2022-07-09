FIND_PACKAGE( Qt4 REQUIRED ) 	# find and setup Qt for this project
IF ( QT4_FOUND )
  SET( QT_USE_QTMAIN TRUE )
  SET( QT_USE_QTXML TRUE )
  INCLUDE( ${QT_USE_FILE} )
ENDIF( QT4_FOUND )

set(matrixio_srcs
	matrixio.cpp
	pointreader.cpp
	datafile.cpp
)

set(matrixio_hdrs
	matrixio.h
	basematrix.hpp
	uppertriangularmatrix.hpp
	squarematrix.hpp
	pointreader.h
	dlldefines.h
	datafile.h
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/Shared
${CMAKE_BINARY_DIR}/Shared
)

add_library(MatrixIO SHARED ${matrixio_srcs} ${matrixio_hdrs})

TARGET_LINK_LIBRARIES(MatrixIO ${QT_LIBRARIES})
