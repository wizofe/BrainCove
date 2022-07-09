include("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/syntaxhighlighter/syntaxhighlighter.cmake")

set(module_name 3rdparty)

#QT4_WRAP_CPP(${module_name}_moc ${${module_name}_hdrs})
#QT4_WRAP_UI(${module_name}_uis_h ${${module_name}_uis})
#QT4_AUTOMOC( ${${module_name}_srcs} )
#QT4_ADD_RESOURCES(editor_rcc_src ${ui_qrc})

set(${module_name}_srcs 
	${syntaxhighlighter_srcs}
)

set(${module_name}_hdrs
	${syntaxhighlighter_hdrs}
)

set(${module_name}_uis
)

set(${module_name}_qrc
)
