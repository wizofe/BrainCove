#ifndef KERNELEDITORFORM_H
#define KERNELEDITORFORM_H

#include <QtGui/QDialog>
#include <QtGui/QFontMetrics>
#include <QtGui/QTableView>
#include "tablemodel.h"
#include <QString>
#include <QFileInfo>
#include <QTextCursor>
#include <ui_kerneleditorform.h>
#include <3rdparty/syntaxhighlighter/qsvsyntaxhighlighter.h>
#include <3rdparty/syntaxhighlighter/qsvlangdef.h>
#include <3rdparty/syntaxhighlighter/qsvcolordef.h>
#include <3rdparty/syntaxhighlighter/qsvcolordeffactory.h>
#include <CLWrappers/CLProgram.h>
#include "../DLLDefines.h"

class KernelEditorForm : public QDialog
{
	Q_OBJECT

private slots:
	void on_saveButton_clicked(bool);
	void on_buildButton_clicked(bool);
	void onsourceChanged();
	void updateCursorPos();
	void on_tableView_activated ( const QModelIndex & index );

public:
	UI_EXPORT KernelEditorForm(QWidget *parent = 0, Qt::WFlags flags = 0);
	UI_EXPORT ~KernelEditorForm();

	UI_EXPORT void setPrograms(CLProgram * progrs, int count);

	//std::vector<CLProgram*> programs() { return m_programs; }

private:
	void Open(std::string fPath);
	void setModified(bool mod);

	Ui::KernelEditorForm ui;

	QsvLangDef *langDef;
	QsvColorDefFactory *colorDef;
	QsvSyntaxHighlighter *highlighter;

	CLProgram *m_programs;
	int program_count;
	CLProgram m_program;

	std::string filePath;

	TableModel *tableModel;
};

#endif // KERNELEDITORFORM_H
