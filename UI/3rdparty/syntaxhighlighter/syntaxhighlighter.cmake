INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/syntaxhighlighter")

set(module_name syntaxhighlighter)

set(${module_name}_srcs
	3rdparty/syntaxhighlighter/qsvcolordef.cpp
	3rdparty/syntaxhighlighter/qsvcolordeffactory.cpp
	3rdparty/syntaxhighlighter/qsvlangdef.cpp
	3rdparty/syntaxhighlighter/qsvlangdeffactory.cpp
	3rdparty/syntaxhighlighter/qsvsyntaxhighlighter.cpp
)

set(${module_name}_hdrs
	3rdparty/syntaxhighlighter/debug_info.h
	3rdparty/syntaxhighlighter/qorderedmap.h
	3rdparty/syntaxhighlighter/qsvcolordef.h
	3rdparty/syntaxhighlighter/qsvcolordeffactory.h
	3rdparty/syntaxhighlighter/qsvlangdef.h
	3rdparty/syntaxhighlighter/qsvlangdeffactory.h
	3rdparty/syntaxhighlighter/qsvsyntaxhighlighter.h
)

set(${module_name}_uis
)

set(${module_name}_qrc
)

QT4_WRAP_CPP(${module_name}_moc ${${module_name}_hdrs})
#QT4_WRAP_UI(${module_name}_uis_h ${${module_name}_uis})
QT4_AUTOMOC( ${${module_name}_srcs} )
#QT4_ADD_RESOURCES(editor_rcc_src ${ui_qrc})