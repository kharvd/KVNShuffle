#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDebug>
#include <ctime>
#include <QFile>
#include <QByteArray>
#include <QSettings>
#include <QMessageBox>
#include <QImageReader>
#include <QCloseEvent>

#include "buttonstablewidget.h"
#include "displaywidget.h"
#include "settingsdialog.h"

const QString MainWindow::iniFile = "settings.ini";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    loadSettings();

    QSettings sets(iniFile, QSettings::IniFormat);
    m_resDir = sets.value("resDir").toString();

    qsrand(time(0));
    loadResources();

    wgtButtons = new ButtonsTableWidget(m_resources.size());
    wgtDisplay = new DisplayWidget();

    ui->stack->addWidget(wgtButtons);
    ui->stack->addWidget(wgtDisplay);

    addAction(ui->actExitFullscreen);

    connect(wgtButtons, SIGNAL(buttonClicked(int)), wgtButtons,
            SLOT(disable(int)));
    connect(wgtButtons, SIGNAL(buttonClicked(int)),
            SLOT(showResource(int)));
    connect(wgtDisplay, SIGNAL(back()), SLOT(backToButtons()));
    connect(ui->actStartOver, SIGNAL(triggered()), SLOT(startOver()));
    connect(ui->actAbout, SIGNAL(triggered()), SLOT(about()));
    connect(ui->actSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui->actFullscreen, SIGNAL(toggled(bool)),
            SLOT(fullscreenToggled(bool)));
    connect(ui->actExitFullscreen, SIGNAL(triggered()),
            SLOT(exitFullscreen()));
    connect(ui->actAboutQt, SIGNAL(triggered()), SLOT(aboutQt()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::loadSettings()
{
    QSettings sets(iniFile, QSettings::IniFormat);
    ui->lblTitle->setText(sets.value("title_text").toString());
}

void MainWindow::settings()
{
    SettingsDialog *dlg = new SettingsDialog(iniFile, this);

    if (dlg->exec()) {
        loadSettings();
    }

    delete dlg;
}

void MainWindow::about()
{
    QMessageBox::about(this, "О программе", "<p>По всем вопросам, "
                       "касающимся этой программы вы можете обращаться по email "
                       "<a href=\"mailto:kharvd@gmail.com\">kharvd@gmail.com</a></p>"

                       "<p>Copyright © 2011 Валерий Харитонов</p>"

                       "<p>Это программа распространяется БЕЗ ВСЯКИХ ГАРАНТИЙ. "
                       "Это свободное программное обеспечение, и Вы можете "
                       "распространять её в соответствии с конкретными условиями. "
                       "Для дополнительной информации смотрите "
                       "<a href=\"http://www.gnu.org/licenses/gpl.html\">"
                       "http://www.gnu.org/licenses/gpl.html</a></p>");
}

void MainWindow::startOver()
{
    backToButtons();

    wgtButtons->clear();
    m_resources.clear();

    loadResources();

    wgtButtons->setButtonsCount(m_resources.size());
}

void MainWindow::backToButtons()
{
    wgtDisplay->clear();
    ui->stack->setCurrentIndex(0);
}

void MainWindow::showResource(int index)
{
    ui->stack->setCurrentIndex(1);

    QString resource;
    resource = m_resources.at(index).second;

    if (m_resources.at(index).first == RES_PICTURE) {
        wgtDisplay->showPixmap(resource);
    } else {
        wgtDisplay->showText(resource);
    }
}

void MainWindow::loadResources()
{
    // Pixmaps
    QStringList filterPics;

    foreach (QString format, QImageReader::supportedImageFormats()) {
        filterPics << "*." + format;
    }

    qDebug() << filterPics;

    QDir resDir(m_resDir);
    QStringList pics = resDir.entryList(filterPics, QDir::Files);

    foreach (QString path, pics) {
        m_resources << qMakePair(RES_PICTURE, resDir.absoluteFilePath(path));
    }

    // Texts
    QStringList txts = resDir.entryList(QStringList() << "*.txt",
                                                QDir::Files);
    foreach (QString path, txts) {
        QFile f(resDir.absoluteFilePath(path));
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        m_resources.append(qMakePair(RES_TEXT, QString(f.readAll())));
    }

    m_resources = shuffled(m_resources);
}

void MainWindow::fullscreenToggled(bool isActivated)
{
    if (isActivated) {
        m_wasMaximized = isMaximized();
        showFullScreen();
    } else {
        showNormal();
        if (m_wasMaximized) {
            showMaximized();
        }
    }
}

void MainWindow::exitFullscreen()
{
    if (isFullScreen()) {
       ui->actFullscreen->toggle();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton btn =
            QMessageBox::question(this, "Выход", "Вы действительно хотите выйти?",
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (btn == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}