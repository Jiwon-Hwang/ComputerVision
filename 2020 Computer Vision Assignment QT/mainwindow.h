#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QPixmap>
#include <QStyle>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void loadButtonSlot();
    void saveButtonSlot();
    void splitButtonSlot();

private:
    QToolBar *toolbar;
    QToolButton *loadButton;
    QToolButton *saveButton;
    QToolButton *splitButton;
    QString imgPath;
    QLabel *imageLabel;
    QPixmap QOrigin;

    cv::Mat origin, *imgs = new cv::Mat[3];
    Ui::MainWindow *ui;

    int imageWidth, imageHeight;

    void setToolBar();
    void setlabel();
    void setApp();
};
#endif // MAINWINDOW_H
