#include "mainwindow.h"

#include <QFileDialog>

extern cv::Mat* assignment(cv::Mat* imgs);
extern bool checkImgs(cv::Mat *imgs);
extern void showRGB(const cv::Mat* imgs);

void MainWindow::loadButtonSlot()
{
    // Load image path and read image to cv::Mat
    imgPath = QFileDialog::getOpenFileName(this, "Select Image", "./", " *.jpg ;; *.png ;; *.bmp ;; *.*");
    if(imgPath.isEmpty()){return;}

    origin = cv::imread(imgPath.toStdString());
    cv::resize(origin, origin ,cv::Size(imageWidth, imageHeight));

    // Change BGR to RGB and make QPixmap, set image on label
    cv::cvtColor(origin, origin, cv::COLOR_BGR2RGB);
    QOrigin = QPixmap::fromImage(QImage(origin.data,
                                        origin.cols,
                                        origin.rows,
                                        QImage::Format_RGB888)); // if use QPixmap pointer, it would occur a swap error
    imageLabel->setPixmap(QOrigin.scaled(imageWidth,
                                         imageHeight,
                                         Qt::KeepAspectRatio)); // keeping image ratio
}

void saveRGB(const std::string path, cv::Mat* imgs)
{
    cv::imwrite(path + "R.jpg", imgs[0]);
    cv::imwrite(path + "G.jpg", imgs[1]);
    cv::imwrite(path + "B.jpg", imgs[2]);
}

void MainWindow::saveButtonSlot()
{
    // if R, G, B are empty, slot would be overed
    if(checkImgs(imgs)) return;

    // save RGB channels
    saveRGB("CV_Assignment_", imgs);
}

void MainWindow::splitButtonSlot()
{
    imgs[0] = imgs[1] = imgs[2] = origin.clone();
    imgs = assignment(imgs);
}

void MainWindow::setToolBar()
{
    // make load_image_button and set Icon
    // the Icon's size is 30 x 30
    // if click and release the icon, it would call the loadImgButtonSlot()
    loadButton = new QToolButton();
    loadButton->setIcon(QIcon(":/image/loadIcon.png"));
    loadButton->setIconSize(QSize(30, 30));
    loadButton->setToolTip("Load an Image");
    connect(loadButton, SIGNAL(released()), this, SLOT(loadButtonSlot()));

    saveButton = new QToolButton();
    saveButton->setIcon(QIcon(":/image/saveIcon.png"));
    saveButton->setIconSize(QSize(30, 30));
    saveButton->setToolTip("Save RGB Images");
    connect(saveButton, SIGNAL(released()), this, SLOT(saveButtonSlot()));

    splitButton = new QToolButton();
    splitButton->setIcon(QIcon(":/image/splitIcon.png"));
    splitButton->setIconSize(QSize(30, 30));
    splitButton->setToolTip("Split RGB Channels");
    connect(splitButton, SIGNAL(released()), this, SLOT(splitButtonSlot()));

    // make toolbar and add widget to toolbar
    toolbar = new QToolBar("tool bar");
    toolbar->addWidget(loadButton);
    toolbar->addWidget(splitButton);
    toolbar->addWidget(saveButton);
}
