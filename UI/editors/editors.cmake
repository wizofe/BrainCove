set(module_name editors)

set(${module_name}_srcs 
	${module_name}/kerneleditorform.cpp
	${module_name}/tablemodel.cpp
)

set(${module_name}_hdrs
	${module_name}/kerneleditorform.h
	${module_name}/tablemodel.h
)

set(${module_name}_uis
	${module_name}/kerneleditorform.ui
)

set(${module_name}_qrc
)

QT4_WRAP_CPP(${module_name}_moc ${${module_name}_hdrs})
QT4_WRAP_UI(${module_name}_uis_h ${${module_name}_uis})
QT4_AUTOMOC( ${${module_name}_srcs} )

message(${CMAKE_CURRENT_BINARY_DIR})

#QT4_ADD_RESOURCES(editor_rcc_src ${ui_qrc})