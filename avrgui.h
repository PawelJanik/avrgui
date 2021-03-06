#ifndef AVRGUI_H
#define AVRGUI_H

#include <QMainWindow>

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <QFileDialog>
#include <QFileSystemModel>
#include <QDir>
#include <QModelIndex>

#include <QMessageBox>

#include <QProcess>
#include <QScrollBar>

#include "project.h"
#include "projectsettingsdialog.h"
#include "newprojectdialog.h"

namespace Ui {
class AvrGui;
}

class AvrGui : public QMainWindow
{
	Q_OBJECT

public:
	explicit AvrGui(QWidget *parent = 0);
	~AvrGui();

protected:
	void closeEvent(QCloseEvent * event) override;

public slots:
	void newFile();
	void newProject();

	void openFile();
	void openProject();

	void save();
	void saveDocument(int index);
	void saveAs();
	void saveProject();

	void closeFile();

	void copy();
	void cut();
	void paste();

	void build();
	void upload();
	void buildAndUpload();

	void cleanProject();
	void rebuild();

	void readProcessData();

	void settings();
	void projectSettings();

	void viewIsChanged();

	void help();
	void about();

	void fileListClicked();
	void documentChanged();
	void documentSaved();

	void projectViewDoubleClick(QModelIndex index);

	void clearOutput();

private:
	Ui::AvrGui *ui;

	KTextEditor::Editor *editor;
	KTextEditor::View *view;

	QList<KTextEditor::Document *> doc;

	int actualOpenDoc;

	void openURL(QString fileName);
	void openProjectUrl(QUrl projectUrl);
	int isOpenFile(QString fileName);

	int documentIsClosing(int index);
	int closeFileAt(int index);

	Project project;
	QFileSystemModel * projectFileModel;

	QProcess * buildProcess;
	QString outputString;

	int buildWarningCounter;
	int buildErrorCounter;
	int uploadFalied;
	bool isBuild;
};

#endif // AVRGUI_H
