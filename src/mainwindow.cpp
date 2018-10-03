/*
 * Copyright (C) 2012-2014 Robert Chéramy (robert@cheramy.net)
 *
 * This file is part of YASW (Yet Another Scan Wizard).
 *
 * YASW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YASW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YASW.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QChar>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QColor>
#include <QtXml/QDomDocument>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->imageList->setFilterContainer(ui->filterContainer);

    showMaximized();

    settings = new QSettings("yasw", "yasw");
    /* update recent projects menu */
    addRecentProject("");

    preferencesDialog = new PreferencesDialog();

//  Transmit global preferences changes to the filter container.
//  The filter container will retransmit the changes to the filter who subscribed to its signal
    connect(preferencesDialog, SIGNAL(selectionColorChanged(QColor)),
            ui->filterContainer, SLOT(setSelectionColor(QColor)));
    connect(preferencesDialog, SIGNAL(backgroundColorChanged(QColor)),
            ui->filterContainer, SLOT(setBackgroundColor(QColor)));
    connect(preferencesDialog, SIGNAL(displayUnitChanged(QString)),
            ui->filterContainer, SLOT(setDisplayUnit(QString)));
    connect(preferencesDialog, SIGNAL(dpiChanged(int)),
            ui->filterContainer, SLOT(setDPI(int)));



    preferencesDialog->setSettings(settings);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
    delete preferencesDialog;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/** \brief Save the current project
*/
void MainWindow::on_action_Save_triggered()
{
    if (projectFileName.length() != 0) {
        saveProjectSettings(projectFileName);
    } else {
        on_action_SaveAs_triggered();
    }
}

/** \brief Save the current project in a new file
*/
void MainWindow::on_action_SaveAs_triggered()
{

    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QDir::currentPath() + "/project.yasw",
                               tr("yasw projects (*.yasw);;All files (* *.*"));

    if (fileName.length() == 0)
        return;

    //If saving to file was successfull, store the project filename in a global variable
    if (saveProjectSettings(fileName))
        setProjectFileName(fileName);
}

/** Save project settings to an XML File

  returns true if successful; false on failure
  This private member is used to avoid dupplicate code betwenn save and save as.
  */
bool MainWindow::saveProjectSettings(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        // Failure
        QMessageBox::critical(this,
            tr("Could not save Project"),
            tr("A problem occured while saving file \"%1\". The project could not be saved")
                          .arg(fileName)
                              );
        return false;
    }

    QDomDocument doc;
    QDomElement yasw = doc.createElement("yasw");
    doc.appendChild(yasw);
    yasw.setAttribute("version", VERSION);

    preferencesDialog->saveProjectParameters(doc, yasw);
    ui->imageList->saveProjectParameters(doc, yasw);

    QTextStream out(&file);
    doc.save(out, 4);
    file.close();

    return true;
}
/* \Brief Sets the current project name in the title bar and inserts it to the recent projects.

    It also saves the current project file name in the property projectFileName for other usages.
*/
void MainWindow::setProjectFileName(QString fileName)
{
    QFileInfo fi(fileName);

    this->projectFileName = fileName;

    if (fileName.length() > 0) {
        setWindowTitle(tr("yasw - %1").arg(fi.fileName()));
        addRecentProject(fileName);
    }
    else
        setWindowTitle(tr("yasw - new project"));
}

void MainWindow::addRecentProject(QString fileName)
{
    QStringList recentProjects;
    recentProjects = settings->value("recent_projects").toStringList();

    // If we have a "valid" filename, update the list; else just update the menu
    //NOTE: perhaps the menu update thing should be done in a separate function
    if (fileName.size() > 0) {
        /* remove the file from the list to avoid duplicates */
        recentProjects.removeAll(fileName);
        recentProjects.prepend(fileName);
    }

    /* reduce size of list to MAX_RECENT_PROJECTS */
    while (recentProjects.size() > MAX_RECENT_PROJECTS)
        recentProjects.removeLast();

    settings->setValue("recent_projects", recentProjects);

    ui->recentProjects->clear();

    if (recentProjects.size() == 0) {
        ui->recentProjects->setEnabled(false);
    } else {
        ui->recentProjects->setEnabled(true);
        QAction *action;
        QString project;
        foreach (project, recentProjects) {
            action = ui->recentProjects->addAction(project);
            connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
        }
    }
}

void MainWindow::loadProject(QString fileName)
{
    QMap<QString, QVariant> settings;
    bool loadingOK = true;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        // Failure
        QMessageBox::critical(this,
            tr("Could not load Project"),
            tr("A problem occured while loading file \"%1\"")
                          .arg(fileName)
                              );
        return;
    }

    QDomDocument doc;
    QString errMsg;
    int errLine, errColumn;
    if (!doc.setContent(&file, false, &errMsg, &errLine, &errColumn)) {
        QMessageBox::critical(this,
                              tr("Could not load Project"),
                              tr("A problem occured while parsing file \"%1\" :\n"
                                 "Line %2, Column %3: %4")
                              .arg(fileName, QString::number(errLine),
                                   QString::number(errColumn), errMsg));
        return;
    }

    QDomElement rootElement = doc.documentElement();
    if (rootElement.tagName() != "yasw") {
        QMessageBox::critical(this,
            tr("Could not load Project"),
            tr("\"%1\" is not a valid YASW project file")
                          .arg(fileName)
                              );
        return;
    }

    // if loadingError is true, the function after || will not be called.
    loadingOK = loadingOK && preferencesDialog->loadProjectParameters(rootElement);
    loadingOK = loadingOK && ui->imageList->loadProjectParameters(rootElement);
    if (loadingOK) {
        setProjectFileName(fileName);
    } else {
        // Something went wrong -> clear the project.
        on_action_Close_triggered();
    }
}


/** \brief Open a project from file
 // FIXME: Load and Check QStream version and yasw Version
 */

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Choose project"),
                        QDir::currentPath(),   // FIXME: save last path
                        tr("yasw projects (*.yasw);;All files (* *.*"));
    if (fileName.length() == 0) // Cancel pressed
        return;

    loadProject(fileName);


}


void MainWindow::exportToJpeg()
{
    QString exportFolder = QFileDialog::getExistingDirectory(this,
                tr("Choose folder for export"),
                QDir::currentPath()  // FIXME: save last path
                );

    if (exportFolder.length() == 0)
        return;

    ui->imageList->exportToFolder(exportFolder);
}

void MainWindow::exportToPdf()
{
    QString pdfFilename = QDir::currentPath() + "/file.pdf";

    if (projectFileName.length() > 0) {
        QFileInfo fi(projectFileName);
        pdfFilename = fi.path() + "/" + fi.baseName() + ".pdf";
    }

    QString exportFile = QFileDialog::getSaveFileName(this, tr("Export to PDF"),
                               pdfFilename,
                               tr("PDF Files (*.pdf);;All files (* *.*"));

    if (exportFile.length() == 0)
        return;

    ui->imageList->exportToPdf(exportFile, preferencesDialog->DPI());
}

/** \brief Close curent project,
    reset settings to default values */
void MainWindow::on_action_Close_triggered()
{
    ui->imageList->clear();
    setProjectFileName("");
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("Yet Another Scan Wizard Version: %1").arg(VERSION),
                       tr("Yet Another Scan Wizard (YASW) is an application used to correct images taken "
                          "with a camera while scanning a book.\n\n"

                          "YASW is written by Robert Cheramy <robert@cheramy.net>\n\n"

                          "YASW is free software: you can redistribute it and/or modify "
                          "it under the terms of the GNU General Public License as published by "
                          "the Free Software Foundation, either version 3 of the License, or "
                          "(at your option) any later version.\n"

                          "YASW is distributed in the hope that it will be useful, "
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                          "GNU General Public License for more details.\n"

                          "You should have received a copy of the GNU General Public License "
                          "along with YASW.  If not, see <http://www.gnu.org/licenses/>.\n\n"

                          "YASW uses icons from the Tango Theme, which is in the public domain."));
}

void MainWindow::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadProject(action->text());
}

void MainWindow::on_action_Preferences_triggered()
{
    preferencesDialog->exec();
}
