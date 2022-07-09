set(module_name styles)

set(${module_name}_srcs 
	${module_name}/View3DInteractorStyle.cpp
	${module_name}/View2DInteractorStyle.cpp
	#${module_name}/LinkedViewInteractorStyle.cpp
	#${module_name}/QInteractorStyle.cpp
)

set(${module_name}_hdrs
	${module_name}/View3DInteractorStyle.h
	${module_name}/View2DInteractorStyle.h
	#${module_name}/LinkedViewInteractorStyle.h
	${module_name}/QInteractorStyle.h
)

set(${module_name}_uis
)

set(${module_name}_qrc
)

QT4_WRAP_CPP(${module_name}_moc ${${module_name}_hdrs})
#message(${${module_name}_moc})
QT4_WRAP_UI(${module_name}_uis_h ${${module_name}_uis})
QT4_AUTOMOC( ${${module_name}_srcs} )

#QT4_ADD_RESOURCES(editor_rcc_src ${ui_qrc})