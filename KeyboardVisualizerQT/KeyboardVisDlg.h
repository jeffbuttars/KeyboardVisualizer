#ifndef KEYBOARDVISDLG_H
#define KEYBOARDVISDLG_H

#include "KeyboardVisualizerCommon/Visualizer.h"
#include "ui_keyboardvisualizer.h"
#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui
{
    class KeyboardVisDlg;
}

class Ui::KeyboardVisDlg : public QMainWindow
{
    Q_OBJECT

public:
    explicit KeyboardVisDlg(QWidget *parent = 0);
    ~KeyboardVisDlg();

    void show();

    void SetVisualizer(Visualizer* v);
    void StartMinimized(boolean startmin);

private slots:
    void update();

    void show_hide();

    void on_backgroundBrightness_valueChanged(const int arg1);

    void on_amplitude_valueChanged(const int arg1);

    void on_averageSize_valueChanged(const int arg1);

    void on_decay_valueChanged(const int arg1);

    void on_delay_valueChanged(const int arg1);

    void on_normalizationOffset_valueChanged(const double arg1);

    void on_normalizationScale_valueChanged(const double arg1);

    void on_animationSpeed_valueChanged(const double arg1);

    void on_FFTWindowMode_currentIndexChanged(int index);

    void on_backgroundMode_currentIndexChanged(int index);

    void on_foregroundMode_currentIndexChanged(int index);

    void on_singleColorMode_currentIndexChanged(int index);

    void on_averageMode_currentIndexChanged(int index);

    void on_pushButton_Save_Settings_clicked();

    void on_reactiveBackground_clicked(bool checked);

    void on_audioDevice_currentIndexChanged(int index);

    void on_filterConstant_valueChanged(const double arg1);

    void on_silentBackground_clicked(bool checked);

    void on_backgroundTimeout_valueChanged(const int arg1);

private:
    Ui::KeyboardVisualizerDlg *ui;
    QSystemTrayIcon* trayIcon;
    QTimer* timer;
    QGraphicsScene* scene;
    QImage* image;
    QPixmap pixmap;
};
#endif // KEYBOARDVISDLG_H
