set(module_name views)

set(${module_name}_srcs 
	${module_name}/renderwidget.cpp
	${module_name}/renderwidget3d.cpp
	${module_name}/mdiwidget.cpp
	${module_name}/renderwidget2d.cpp
	${module_name}/renderwidgetslices.cpp
	${module_name}/sliceviewswidget.cpp
)

set(${module_name}_hdrs
	${module_name}/renderwidget.h
	${module_name}/renderwidget3d.h
	${module_name}/mdiwidget.h
	${module_name}/renderwidget2d.h
	${module_name}/renderwidgetslices.h
	${module_name}/sliceviewswidget.h
)

set(${module_name}_uis
)

set(${module_name}_qrc
)

QT4_WRAP_CPP(${module_name}_moc ${${module_name}_hdrs})
QT4_WRAP_UI(${module_name}_uis_h ${${module_name}_uis})
QT4_AUTOMOC( ${${module_name}_srcs} )

#QT4_ADD_RESOURCES(editor_rcc_src ${ui_qrc})