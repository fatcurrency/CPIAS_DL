#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "view/singleviewslicevisualization.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui;
    // 2D Slice
    QVBoxLayout *sliceLayout;
    SingleViewSliceVisualization *sliceView;
};

#endif // MAINWINDOW_H
