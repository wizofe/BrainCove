#include "kerneleditorform.h"

KernelEditorForm::KernelEditorForm(QWidget *parent, Qt::WFlags flags) 
: QDialog(parent, flags) 
{
	ui.setupUi(this);
	ui.buildLogWidget->setVisible(false);

	tableModel = new TableModel(this);

	langDef = new QsvLangDef(":/filetypes/c.lang");
	colorDef = new QsvColorDefFactory(":/filetypes/kate.xml" );

	highlighter = new QsvSyntaxHighlighter(ui.textEdit,colorDef,langDef);

	connect(ui.textEdit->document(),SIGNAL(contentsChanged()),this,SLOT(onsourceChanged()));
	connect(ui.textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(updateCursorPos()));

	QFontMetrics metrics(ui.textEdit->font());
	ui.textEdit->setTabStopWidth(metrics.width(' ')*4);

	ui.tableView->setModel(this->tableModel);

}

KernelEditorForm::~KernelEditorForm()
{
}

void KernelEditorForm::Open(std::string fPath)
{
	filePath = fPath;
	std::ifstream fin(filePath.c_str());
	fin.seekg(0, std::ios_base::end);
	std::vector<char> buffer(fin.tellg());
	fin.seekg(0);
	fin.read(&buffer[0], buffer.size());

	std::string source(&buffer[0], buffer.size());

	fin.close();

	ui.textEdit->document()->clear();
	ui.textEdit->document()->setPlainText(QString::fromStdString(source));

	highlighter->setDocument(ui.textEdit->document());

	highlighter->rehighlight();

	setModified(false);
}

void KernelEditorForm::setPrograms(CLProgram *progrs, int count)
{
	m_programs = progrs;
	program_count = count;

	for(int i = 0; i < count; i++)
	{
		QString fileName = QFileInfo(QString::fromStdString(m_programs[i].GetProgramFile())).fileName();
		std::vector<std::string> activeKernels = m_programs[i].GetActiveKernels();
		QString kernelNames;
		for(int j = 0;j<activeKernels.size();j++)
			kernelNames += QString::fromStdString(activeKernels.at(j)) + " ";
		tableModel->appendRow(QPair<QString,QString>(fileName,kernelNames));

	}
	//Open(m_program->programFile());
}

void KernelEditorForm::setModified(bool mod)
{
	if(mod)
		setWindowTitle(tr("*%1 - kernel editor").arg(filePath.c_str()));
	else
		setWindowTitle(tr("%1 - kernel editor").arg(filePath.c_str()));
}

void KernelEditorForm::on_saveButton_clicked(bool)
{
	std::ofstream fout(m_program.GetProgramFile().c_str());

	fout << ui.textEdit->document()->toPlainText().toStdString();

	fout.close();

	setModified(false);
}

void KernelEditorForm::on_buildButton_clicked(bool)
{
	on_saveButton_clicked(true);

	int err = m_program.Rebuild();
	if(err)
	{
		ui.errorLabel->setText(tr("Build error: %1").arg(err));
		ui.buildLogWidget->setVisible(true);
		ui.buildLog->setPlainText(QString::fromStdString(m_program.programBuildLog()));
	}
	else
		ui.buildLogWidget->setVisible(false);
}

void KernelEditorForm::on_tableView_activated ( const QModelIndex & index )
{
	m_program = m_programs[index.row()];
	Open(m_program.GetProgramFile());
}

void KernelEditorForm::onsourceChanged()
{
	setModified(true);
}

void KernelEditorForm::updateCursorPos()
{
	/*QTextBlock b, cb;
	QTextDocument *doc = ui.textEdit->document();
	int col = ui.textEdit->textCursor().columnNumber();
	int line = ui.textEdit->textCursor().blockNumber();

	ui.label->setText(QString("%1,%2").arg(col).arg(line));
	*/
}
