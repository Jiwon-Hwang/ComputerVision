#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setApp();
    setlabel();
    setToolBar();

    this->addToolBar(Qt::TopToolBarArea, toolbar);
}

void MainWindow::setApp()
{
    /*
     * change window name
     * set MainWindow size and alinged it
     * use monitor width, height
     */
    this->setWindowTitle("Computer Vision Assigment 1");

    QDesktopWidget d;

    // get monitor size
    // if an user use dual monitors, application width would be divided by 2
    double dw = d.width() < d.height() * 2 ? d.width() * 0.7 : d.width() / 2 * 0.7;
    double dy = d.height() * 0.7;

    // set window size
    setGeometry( QStyle::alignedRect(Qt::LeftToRight,
                                     Qt::AlignCenter,
                                     QSize(dw, dy),
                                     qApp->desktop()->availableGeometry()));
}

MainWindow::~MainWindow()
{
    cv::destroyAllWindows();
    delete ui;
}
