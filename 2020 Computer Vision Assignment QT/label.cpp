#include "mainwindow.h"

void MainWindow::setlabel()
{
    imageWidth = this->width() - 20;
    imageHeight = this->height() - 55;

    imageLabel = this->findChild<QLabel*>("originLabel");
    imageLabel->setGeometry( QStyle::alignedRect(Qt::LeftToRight,
                                                 Qt::AlignCenter,
                                                 QSize(imageWidth, imageHeight),
                                                 qApp->desktop()->availableGeometry()));
}
