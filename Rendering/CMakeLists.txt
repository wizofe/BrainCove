# --------------------------------------------------
# Project name
SET( ProjectName Rendering )
PROJECT( ${ProjectName} )

SET(USE_GPU ON CACHE BOOL "Sets whether to use the GPU")

IF(USE_NQVTK)
	FIND_PACKAGE( NQVTK REQUIRED ) # find and setup NQVTK for this project\
	FIND_PACKAGE( NQVTKGui REQUIRED ) # find and setup NQVTK for this project
	INCLUDE( ${NQVTK_USE_FILE} )
	INCLUDE( ${NQVTKGui_USE_FILE} )
ENDIF(USE_NQVTK)

IF(WIN32)
	FIND_PACKAGE( GLEW REQUIRED ) #we need glew on Windows for glTexture3D...
ENDIF(WIN32)

FIND_PACKAGE( VTK REQUIRED ) 	# find and setup NQVTK for this project
FIND_PACKAGE( Qt4 REQUIRED ) 	# find and setup Qt for this project
FIND_PACKAGE( OpenCL REQUIRED ) # find and setup OpenCL for this project
FIND_PACKAGE( OpenGL REQUIRED ) # find and setup OpenGL for this project

INCLUDE( ${VTK_USE_FILE} ) 
IF ( QT4_FOUND )
  SET( QT_USE_QTMAIN TRUE )
  SET( QT_USE_QTOPENGL TRUE )
  INCLUDE( ${QT_USE_FILE} )
ENDIF( QT4_FOUND )

IF( GLEW_FOUND )
  INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
ENDIF( GLEW_FOUND )

IF( OPENCL_FOUND )
  INCLUDE_DIRECTORIES( ${_OPENCL_CPP_INCLUDE_DIRS} )
ENDIF( OPENCL_FOUND )

FIND_PATH(OPENCL_KERNELS_DIR NAMES *.cl PATHS ${CMAKE_CURRENT_SOURCE_DIR})

ADD_DEFINITIONS(-DKERNEL_DIR=\"${OPENCL_KERNELS_DIR}\")

IF( USE_GPU )
	ADD_DEFINITIONS(-DPREFERRED_DEVICE_TYPE=CL_DEVICE_TYPE_GPU)
ELSE (USE_GPU )
	ADD_DEFINITIONS(-DPREFERRED_DEVICE_TYPE=CL_DEVICE_TYPE_CPU)
ENDIF(USE_GPU)

# --------------------------------------------------
# Modules

SET( ${ProjectName}_SourceBaseDir
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

SET( ${ProjectName}_Modules
	CLWrappers
	Filters
	Layouts
	Selectors
	Systems
	Renderables
	Mappers
	Utils
)

# Friendly names for the modules
SET( ${ProjectName}_ModuleNames
	"Wrappers for cl mem objects"
	"Render filters"
	"Render layouts"
	"Render selections"
	"Render systems"
	"Renderables"
	"Renderable mappers"
	"Utility classes"
)

set(${ProjectName}_srcs
)

set(${ProjectName}_hdrs
	dlldefines.h
)

set(${ProjectName}_rcc 
#	resources.qrc
)

SET( ${ProjectName}_CLWrappers_Classes
	CLMemoryObject
	CLBufferObject
	CLTexture2DObject
	CLTexture3DObject
	CLProgram
	CLContext
)

SET( ${ProjectName}_Filters_Classes
	DataFilter
)

SET( ${ProjectName}_Selectors_Classes
	DataSelection
	VoxelSelection
	VoxelVoxelSelection
	MatrixVoxelSelector
	BaseVoxelSelector
	BrainVoxelSelector
)

SET( ${ProjectName}_Layouts_Classes
	BaseLayout
	BrainLayout
	BrainLayout1D
)

SET( ${ProjectName}_Systems_Classes
	VTKRenderSystem
	ARenderSystem
)

SET( ${ProjectName}_Renderables_Classes
	ARenderable
	CorrelationVolume
	VTKRenderable
)

SET( ${ProjectName}_Mappers_Classes
	GPUGenericRayCastMapper
	GPUPlaneRayCastMapper
	GPUVolumeRayCastMapper
	CLRayCaster
	CLTwoPassRayCaster
	#CLHighResRayCaster
)

SET( ${ProjectName}_Utils_Classes
	clext
	ColorMapping
	CLTexturePainter
	PointsToVolumeData
)

IF(USE_NQVTK)
SET( ${ProjectName}_Systems_Classes
	${${ProjectName}_Systems_Classes}
	NQVTKRenderSystem
)
SET( ${ProjectName}_Renderables_Classes
	${${ProjectName}_Renderables_Classes}
	NQVTKRenderable
)	

ENDIF(USE_NQVTK)

SET( ${ProjectName}_Source ${${ProjectName}_srcs} ${${ProjectName}_rcc} ${${ProjectName}_hdrs})

#SOURCE_GROUP(Layout FILES ${rendering.layout_srcs} ${rendering.layout_hdrs})

# --------------------------------------------------
# Macros

MACRO( collect_class_source_files module class headers sources )
	# Find header file
	SET( headerfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.h" )
	IF( EXISTS ${headerfile} )
		SET( ${headers} ${${headers}} ${headerfile} )
	ELSE( EXISTS ${headerfile} )
		SET( headerfile "${${ProjectName}_SourceBaseDir}/${class}.h" )
		IF( EXISTS ${headerfile} )
			SET( ${headers} ${${headers}} ${headerfile} )
		ELSE( EXISTS ${headerfile} )
			MESSAGE( SEND_ERROR "Class header not found for '${class}'" )
		ENDIF( EXISTS ${headerfile} )
	ENDIF( EXISTS ${headerfile} )

	# Find source files
	SET( classfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.cpp" )
	IF( EXISTS ${classfile} )
		SET( ${sources} ${${sources}} "${classfile}" )
	ELSE( EXISTS ${classfile} )
		SET( classfile "${${ProjectName}_SourceBaseDir}/${module}/${class}.cxx" )
		IF( EXISTS ${classfile} )
			SET( ${sources} ${${sources}} "${classfile}" )
		ELSE( EXISTS ${classfile} )
			SET( classfile "${${ProjectName}_SourceBaseDir}/${class}.cpp" )
			IF( EXISTS ${classfile} )
				SET( ${sources} ${${sources}} "${classfile}" )
			ELSE( EXISTS ${classfile} )
				SET( classfile "${${ProjectName}_SourceBaseDir}/${class}.cxx" )
				IF( EXISTS ${classfile} )
					SET( ${sources} ${${sources}} "${classfile}" )
				ELSE( EXISTS ${classfile} )
					# Allow header-only implementations
					# MESSAGE( SEND_ERROR "Class implementation not found for '${class}'" )
				ENDIF( EXISTS ${classfile} )
			ENDIF( EXISTS ${classfile} )
		ENDIF( EXISTS ${classfile} )
	ENDIF( EXISTS ${classfile} )
ENDMACRO( collect_class_source_files )

# --------------------------------------------------
# Collect source files and create source groups

# Iterate over the modules
LIST( LENGTH ${ProjectName}_Modules nmodules )
MATH( EXPR nmodules "${nmodules} - 1" )
FOREACH( i RANGE ${nmodules} )

	# Fetch name and label of the module
	LIST( GET ${ProjectName}_Modules ${i} module )
	LIST( GET ${ProjectName}_ModuleNames ${i} modulename )

	# See if there is a separate CMake include for this module
	SET( cmakeincludefile "${${ProjectName}_SourceBaseDir}/${module}/sources.cmake" )
	IF( EXISTS ${cmakeincludefile} )
		# Include the file
		INCLUDE( ${cmakeincludefile} )
	ELSE( EXISTS ${cmakeincludefile} )
		# Iterate over classes
		SET( ${ProjectName}_${module}_Headers )
		SET( ${ProjectName}_${module}_Source )
		SET( ${ProjectName}_${module}_GeneratedSource )

		FOREACH( class ${${ProjectName}_${module}_Classes} )
			collect_class_source_files( 
				${module} 
				${class} 
				${ProjectName}_${module}_Headers 
				${ProjectName}_${module}_Source 
			)
		ENDFOREACH( class )
	ENDIF( EXISTS ${cmakeincludefile} )

	# Create source group for this module
	# Generated sources don't need to show up in this group, headers do
	SOURCE_GROUP( ${modulename} 
		FILES ${${ProjectName}_${module}_Source} ${${ProjectName}_${module}_Headers} 
	)
	# Append files to complete sets for the project
	SET( ${ProjectName}_Headers 
		${${ProjectName}_Headers} 
		${${ProjectName}_${module}_Headers} 
	)
	SET( ${ProjectName}_Source
		${${ProjectName}_Source} 
		${${ProjectName}_${module}_Source} 
		${${ProjectName}_${module}_Headers} # Add headers as well
	)
	SET( ${ProjectName}_GeneratedSource
		${${ProjectName}_GeneratedSource}
		${${ProjectName}_${module}_GeneratedSource}
	)
ENDFOREACH( i )

# Add generated sources and give them a source group
SET( ${ProjectName}_Source
	${${ProjectName}_Source}
	${${ProjectName}_GeneratedSource}
)
SOURCE_GROUP( "Generated files"
	FILES ${${ProjectName}_GeneratedSource}
)

LINK_DIRECTORIES(${VTK_BINARY_PATH})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/Shared
${CMAKE_BINARY_DIR}/Shared
${CMAKE_SOURCE_DIR}/MatrixIO
)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${${ProjectName}_SourceBaseDir} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${${ProjectName}_Dependency_Includes} )

QT4_ADD_RESOURCES(rendering_rcc_src ${rendering_rcc})
#QT4_AUTOMOC( ${${ProjectName}_Source} )
#QT4_WRAP_CPP(bla ${${ProjectName}_SourceBaseDir}/Systems/ARenderSystem.h)

#message(${bla})

add_library(${ProjectName} SHARED ${${ProjectName}_Source}) #${bla})

TARGET_LINK_LIBRARIES(${ProjectName}
	MatrixIO
    ${GLEW_LIBRARY}
    ${VTK_LIBS}
	vtkCommon
	vtkHybrid
	vtkFiltering
	vtkVolumeRendering
	QVTK
	${QT_LIBRARIES}
	${OPENCL_LIBRARIES}
	${OPENGL_LIBRARY}
)
