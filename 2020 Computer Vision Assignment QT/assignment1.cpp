#include "mainwindow.h"

void showRGB(const cv::Mat* imgs)
{
    cv::imshow("R", imgs[0]);
    cv::imshow("G", imgs[1]);
    cv::imshow("B", imgs[2]);
}

bool checkImgs(cv::Mat *imgs)
{
    // if one of channels is empyt, return true;
    return imgs[0].empty() && imgs[1].empty() && imgs[2].empty();
}

cv::Mat* assignment(cv::Mat* imgs)
{
    Q_ASSERT(!checkImgs(imgs));

    showRGB(imgs);

    return imgs;
}
