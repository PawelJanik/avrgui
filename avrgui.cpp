#include "avrgui.h"
#include "ui_avrgui.h"

AvrGui::AvrGui(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::AvrGui)
{
	ui->setupUi(this);

	editor = KTextEditor::Editor::instance();

	actualOpenDoc = 0;

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(ui->actionNewProject, SIGNAL(triggered()), this, SLOT(newProject()));

	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui->actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()));

	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
	connect(ui->actionSaveProject, SIGNAL(triggered()), this, SLOT(saveProject()));

	connect(ui->actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));

	connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
	connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()));
	connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));

	connect(ui->actionBuild, SIGNAL(triggered()), this, SLOT(build()));
	connect(ui->actionUpload, SIGNAL(triggered()), this, SLOT(upload()));
	connect(ui->actionBuildAndUpload, SIGNAL(triggered()), this, SLOT(buildAndUpload()));

	connect(ui->actionCleanProject, SIGNAL(triggered()), this, SLOT(cleanProject()));
	connect(ui->actionRebuild, SIGNAL(triggered()), this, SLOT(rebuild()));

	connect(ui->clearOutputButton, SIGNAL(clicked(bool)), this, SLOT(clearOutput()));

	connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settings()));
	connect(ui->actionProjectSettings, SIGNAL(triggered()), this, SLOT(projectSettings()));

	connect(ui->actionViewFile, SIGNAL(triggered()), this, SLOT(viewIsChanged()));
	connect(ui->actionViewProject, SIGNAL(triggered()), this, SLOT(viewIsChanged()));
	connect(ui->actionViewOutput, SIGNAL(triggered()), this, SLOT(viewIsChanged()));

	connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui->fileWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(fileListClicked()));
	connect(ui->projectView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(projectViewDoubleClick(QModelIndex)));
}

AvrGui::~AvrGui()
{
	delete ui;
}

void AvrGui::closeEvent(QCloseEvent *event)
{
	bool accept = true;

	for(int i = 0; i < doc.count(); i++)
	{
		int result = closeFileAt(i);
		if(result == QMessageBox::Cancel)
			accept = false;
	}

	if (accept)
		event->accept();
	else
		event->ignore();
}

void AvrGui::newFile()
{
	ui->statusBar->showMessage("New file");

	doc.append(editor->createDocument(0));
	view = doc.last()->createView(this);
	setCentralWidget(view);

	ui->fileWidget->addItem("no named");
	ui->fileWidget->setCurrentRow(doc.count());

	connect(doc.last(), SIGNAL(modifiedChanged(KTextEditor::Document *)), this, SLOT(documentChanged()));
	connect(doc.last(), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));

	ui->fileWidget->setCurrentRow((ui->fileWidget->count())-1);
}

void AvrGui::newProject()
{
	ui->statusBar->showMessage("New project");

	newProjectDialog * newprojectdialog = new newProjectDialog(this);

	int result = newprojectdialog->exec();

	if(result == 1)
	{
		QString projectFile(newprojectdialog->project.path.path().append("/").append(newprojectdialog->project.name).append(".avrgui"));
		openProjectUrl(projectFile);
	}
}

void AvrGui::openFile()
{
	ui->statusBar->showMessage("Open file");

	QString fileName = QFileDialog::getOpenFileName(this,tr("Open file..."), "/home", tr("Source Files (*.c *.cpp *.h)"));

	openURL(fileName);
}

void AvrGui::openURL(QString fileName)
{
	int isOpen = isOpenFile(fileName);

	if(!fileName.isNull() && (isOpen == (-1)))
	{
		QString stringURL = "file:";
		stringURL.append(fileName);
		QUrl url = QUrl(stringURL);

		doc.append(editor->createDocument(0));
		view = doc.last()->createView(this);
		setCentralWidget(view);

		doc.last()->openUrl(url);

		ui->fileWidget->addItem(url.fileName());
		ui->fileWidget->setCurrentRow(doc.count());

		connect(doc.last(), SIGNAL(modifiedChanged(KTextEditor::Document *)), this, SLOT(documentChanged()));
		connect(doc.last(), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));
	}
	else
	{
		actualOpenDoc = isOpen;
		ui->fileWidget->setCurrentRow(isOpen);
		view = doc.at(actualOpenDoc)->createView(this);
		setCentralWidget(view);
	}
}

void AvrGui::openProject()
{
	ui->statusBar->showMessage("Open project");

	QString filename = QFileDialog::getOpenFileName(this,tr("Open project..."), "/home", tr("Projects files (*.avrgui)"));

	openProjectUrl(QUrl(filename));
}

void AvrGui::openProjectUrl(QUrl projectUrl)
{
	if(!projectUrl.isEmpty())
	{
		ui->statusBar->showMessage(QString("Open project from URL: ").append(projectUrl.path()));
		project.setProjectData(projectUrl);
		//ui->output->setText(project.showProjectData());

		projectFileModel = new QFileSystemModel;
		ui->projectView->setModel(projectFileModel);
		ui->projectView->setRootIndex(projectFileModel->setRootPath(project.path.path()));

		ui->projectView->hideColumn(1);
		ui->projectView->hideColumn(2);
		ui->projectView->hideColumn(3);

		openURL(QString(project.path.path()).append("/").append(project.fileList.at(0)));
	}
}

int AvrGui::isOpenFile(QString fileName)
{
	int result = -1;

	for(int i = 0; i < doc.count(); i++)
	{
		if(fileName == doc.at(i)->url().path())
		{
			result = i;
		}
	}

	return result;
}

void AvrGui::save()
{
	ui->statusBar->showMessage("Save");

	doc.at(actualOpenDoc)->documentSave();
}

void AvrGui::saveDocument(int index)
{
	ui->statusBar->showMessage("Save document");

	doc.at(index)->documentSave();
}

void AvrGui::saveAs()
{
	ui->statusBar->showMessage("Save as...");
	doc.at(actualOpenDoc)->documentSaveAs();
}

void AvrGui::saveProject()
{
	ui->statusBar->showMessage("Save project");
}

void AvrGui::closeFile()
{
	if(!doc.isEmpty())
	{
		ui->statusBar->showMessage("Close file");

		closeFileAt(ui->fileWidget->currentRow());
	}
}

int AvrGui::closeFileAt(int index)
{
	int result = documentIsClosing(index);

	if(result != QMessageBox::Cancel)
	{
		ui->fileWidget->takeItem(index);
		doc.removeAt(index);

		if(!doc.isEmpty())
		{
			actualOpenDoc = ui->fileWidget->currentRow();
			view = doc.at(actualOpenDoc)->createView(this);
			setCentralWidget(view);
		}

		if(ui->fileWidget->count() == 0)
		{
			//takeCentralWidget();
			//setCentralWidget(ui->centralWidget);
		}
	}

	return result;
}

int AvrGui::documentIsClosing(int index)
{
	ui->statusBar->showMessage("Document is closing");

	int result = QMessageBox::Save;

	QString fileName = ui->fileWidget->item(index)->text();

	ui->statusBar->showMessage(fileName);

	if(fileName.contains("*",Qt::CaseInsensitive))
	{
		QMessageBox saveMessage;
		saveMessage.setText(QString("Document: ").append(doc.at(index)->documentName()).append( " has been modified."));
		saveMessage.setInformativeText("Do you want to save your changes?");
		saveMessage.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		saveMessage.setDefaultButton(QMessageBox::Save);
		result = saveMessage.exec();

		if(result == QMessageBox::Save)
			saveDocument(index);
	}

	return result;
}

void AvrGui::copy()
{
	ui->statusBar->showMessage("Copy");
}

void AvrGui::cut()
{
	ui->statusBar->showMessage("Cut");
}

void AvrGui::paste()
{
	ui->statusBar->showMessage("Paste");
}

void AvrGui::build()
{
	ui->statusBar->showMessage("Bulid");

	for(int i = 0; i < doc.count(); i++)
	{
		saveDocument(i);
	}

	buildWarningCounter = 0;
	buildErrorCounter = 0;

	clearOutput();
	ui->outputLed->setColor(Qt::gray);
	ui->outputLabel->setText("Program is building...");

	buildProcess = new QProcess(0);
	buildProcess->setWorkingDirectory(project.path.path());

	connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
	connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

	QStringList arg;

	buildProcess->start("make", arg);

	buildProcess->waitForFinished();

	delete buildProcess;

	buildWarningCounter += outputString.count("warning",Qt::CaseInsensitive);

	buildErrorCounter += outputString.count("error",Qt::CaseInsensitive);

	if((buildErrorCounter == 0) && (buildWarningCounter == 0))
	{
		ui->outputLed->setColor(Qt::green);
		ui->outputLabel->setText("Program was build.");
		isBuild = true;
	}
	else if((buildErrorCounter == 0) && (buildWarningCounter > 0))
	{
		ui->outputLed->setColor(Qt::yellow);
		ui->outputLabel->setText(QString("Program was built with ")
					 .append(QString::number(buildWarningCounter))
					 .append(" warnings."));
		isBuild = true;
	}
	else if((buildErrorCounter > 0) && (buildWarningCounter == 0))
	{
		ui->outputLed->setColor(Qt::red);
		ui->outputLabel->setText(QString("Program has ")
					 .append(QString::number(buildErrorCounter))
					 .append(" errors."));
		isBuild = false;
	}
	else if((buildErrorCounter > 0) && (buildWarningCounter > 0))
	{
		ui->outputLed->setColor(Qt::red);
		ui->outputLabel->setText(QString("Program has ")
					 .append(QString::number(buildErrorCounter))
					 .append(" errors, and ")
					 .append(QString::number(buildWarningCounter))
					 .append(" warnings."));
		isBuild = false;
	}
}

void AvrGui::upload()
{
	ui->statusBar->showMessage("Upload");

	if((buildErrorCounter == 0) && (isBuild == true))
	{
		clearOutput();
		uploadFalied = 0;
		ui->outputLed->setColor(Qt::gray);
		ui->outputLabel->setText("Program is uploading...");

		buildProcess = new QProcess(0);
		buildProcess->setWorkingDirectory(project.path.path());

		connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
		connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

		QStringList arg;
		arg << "flash";

		buildProcess->start("make", arg);

		buildProcess->waitForFinished();

		delete buildProcess;

		if(outputString.contains("avrdude: AVR device not responding",Qt::CaseInsensitive))
		{
			uploadFalied++;
			ui->outputLed->setColor(Qt::red);
			ui->outputLabel->setText("Check the device connection or program configuration.");
		}

		if(uploadFalied == 0)
		{
			ui->outputLed->setColor(Qt::green);
			ui->outputLabel->setText("Program was uploaded.");
		}
	}
	else
	{
		build();
		if(buildErrorCounter == 0)
			upload();
	}
}

void AvrGui::buildAndUpload()
{
	ui->statusBar->showMessage("Build and Upload");

	build();

	if(buildErrorCounter == 0)
		upload();
}

void AvrGui::cleanProject()
{
	ui->statusBar->showMessage("Clean project");

	buildProcess = new QProcess(0);
	buildProcess->setWorkingDirectory(project.path.path());

	connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
	connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

	QStringList arg;
	arg << "clean";

	buildProcess->start("make", arg);

	buildProcess->waitForFinished();

	delete buildProcess;

	ui->outputLed->setColor(Qt::green);
	ui->outputLabel->setText("Project is clean.");
}

void AvrGui::rebuild()
{
	ui->statusBar->showMessage("Rebuild");

	cleanProject();
	build();
}

void AvrGui::readProcessData()
{
	ui->statusBar->showMessage("Read process data");

	outputString.append(buildProcess->readAllStandardError());
	outputString.append(buildProcess->readAllStandardOutput());

	ui->output->setText(outputString);
	ui->output->verticalScrollBar()->setValue(ui->output->verticalScrollBar()->maximum());
}

void AvrGui::clearOutput()
{
	ui->output->clear();
	outputString.clear();
}

void AvrGui::settings()
{
	ui->statusBar->showMessage("Settings");
}

void AvrGui::projectSettings()
{
	ui->statusBar->showMessage("Project settings");
}

void AvrGui::viewIsChanged()
{
	ui->statusBar->showMessage("Change view");
}

void AvrGui::help()
{
	ui->statusBar->showMessage("Help");
}

void AvrGui::about()
{
	ui->statusBar->showMessage("About");
}

void AvrGui::fileListClicked()
{
	actualOpenDoc = ui->fileWidget->currentRow();
	view = doc.at(actualOpenDoc)->createView(this);
	setCentralWidget(view);

	ui->statusBar->showMessage("File list was clicked");
}

void AvrGui::documentChanged()
{
	ui->statusBar->showMessage("Document was changed");
	doc.at(actualOpenDoc)->setModified(false);

	ui->fileWidget->takeItem(actualOpenDoc);
	QString filename = doc.at(actualOpenDoc)->url().fileName();
	if(filename.isEmpty() == true)
		filename = "nonamed";
	filename.append("*");
	ui->fileWidget->insertItem(actualOpenDoc,filename);
	ui->fileWidget->setCurrentRow(actualOpenDoc);

	isBuild = false;
}

void AvrGui::documentSaved()
{
	ui->statusBar->showMessage("Document was saved");

	ui->fileWidget->takeItem(actualOpenDoc);
	ui->fileWidget->insertItem(actualOpenDoc,doc.at(actualOpenDoc)->url().fileName());
	ui->fileWidget->setCurrentRow(actualOpenDoc);
}

void AvrGui::projectViewDoubleClick(QModelIndex index)
{
	ui->statusBar->showMessage(QString("Project tree was double clicked. Path: ").append(projectFileModel->filePath(index)));

	openURL(projectFileModel->filePath(index));
}
