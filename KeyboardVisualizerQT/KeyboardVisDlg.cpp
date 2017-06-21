#include "KeyboardVisDlg.h"
#include "KeyboardVisualizerCommon/Visualizer.h"
#include "KeyboardVisualizerCommon/VisualizerDefines.h"
#include "ui_keyboardvisualizer.h"

Visualizer* vis_ptr;
boolean startminimized;
boolean firstrun;

using namespace Ui;

KeyboardVisDlg::KeyboardVisDlg(QWidget *parent) : QMainWindow(parent), ui(new KeyboardVisualizerDlg)
{
    startminimized = false;
    firstrun = true;

    ui->setupUi(this);
    QIcon icon(":Icon.png");
    setWindowIcon(icon);

    QAction* actionExit = new QAction( "Show/Hide", this );
    connect( actionExit, SIGNAL( triggered() ), this, SLOT( show_hide() ));

    QMenu* myTrayIconMenu = new QMenu( this );
    myTrayIconMenu->addAction( actionExit );

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Keyboard Visualizer");
    trayIcon->setContextMenu(myTrayIconMenu);
    trayIcon->show();
}

void KeyboardVisDlg::show()
{
    QMainWindow::show();
}

KeyboardVisDlg::~KeyboardVisDlg()
{
    delete ui;
}

void KeyboardVisDlg::show_hide()
{
    if(isHidden())
    {
        timer->start(15);
        show();
    }
    else
    {
        hide();
        timer->stop();
    }
}

void KeyboardVisDlg::update()
{
    if (startminimized && firstrun)
    {
        firstrun = false;
        hide();
        timer->stop();
    }

    for(int x = 0; x < 256; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            COLORREF input = vis_ptr->pixels_out->pixels[y][x];
            COLORREF bgr = RGB(GetBValue(input), GetGValue(input), GetRValue(input));
            image->setPixel(x, y, bgr);
        }
    }

    pixmap.convertFromImage(*image);
    scene->clear();
    scene->addPixmap(pixmap);

    ui->visualizationPreview->show();

    if(vis_ptr->update_ui)
    {
        vis_ptr->update_ui = false;

        ui->normalizationOffset->setValue(vis_ptr->nrml_ofst);
        ui->normalizationScale->setValue(vis_ptr->nrml_scl);
        ui->animationSpeed->setValue(vis_ptr->anim_speed);
        ui->filterConstant->setValue(vis_ptr->filter_constant);

        ui->amplitude->setValue(vis_ptr->amplitude);
        ui->backgroundBrightness->setValue(vis_ptr->bkgd_bright);
        ui->averageSize->setValue(vis_ptr->avg_size);
        ui->decay->setValue(vis_ptr->decay);
        ui->delay->setValue(vis_ptr->delay);
        ui->backgroundTimeout->setValue(vis_ptr->background_timeout);

        ui->averageMode->blockSignals(true);
        ui->averageMode->setCurrentIndex(vis_ptr->avg_mode);
        ui->averageMode->blockSignals(false);

        ui->backgroundMode->blockSignals(true);
        ui->backgroundMode->setCurrentIndex(vis_ptr->bkgd_mode);
        ui->backgroundMode->blockSignals(false);

        ui->FFTWindowMode->blockSignals(true);
        ui->FFTWindowMode->setCurrentIndex(vis_ptr->window_mode);
        ui->FFTWindowMode->blockSignals(false);

        ui->foregroundMode->blockSignals(true);
        ui->foregroundMode->setCurrentIndex(vis_ptr->frgd_mode);
        ui->foregroundMode->blockSignals(false);

        ui->singleColorMode->blockSignals(true);
        ui->singleColorMode->setCurrentIndex(vis_ptr->single_color_mode);
        ui->singleColorMode->blockSignals(false);

        ui->reactiveBackground->setChecked(vis_ptr->reactive_bkgd);
        ui->silentBackground->setChecked(vis_ptr->silent_bkgd);
    }
}

void KeyboardVisDlg::SetVisualizer(Visualizer* v)
{
    vis_ptr = v;

    ui->normalizationOffset->setValue(vis_ptr->nrml_ofst);
    ui->normalizationScale->setValue(vis_ptr->nrml_scl);
    ui->animationSpeed->setValue(vis_ptr->anim_speed);
    ui->filterConstant->setValue(vis_ptr->filter_constant);

    ui->amplitude->setValue(vis_ptr->amplitude);
    ui->backgroundBrightness->setValue(vis_ptr->bkgd_bright);
    ui->averageSize->setValue(vis_ptr->avg_size);
    ui->decay->setValue(vis_ptr->decay);
    ui->delay->setValue(vis_ptr->delay);
    ui->backgroundTimeout->setValue(vis_ptr->background_timeout);

    ui->FFTWindowMode->blockSignals(true);
    ui->FFTWindowMode->addItem("None");
    ui->FFTWindowMode->addItem("Hanning");
    ui->FFTWindowMode->addItem("Hamming");
    ui->FFTWindowMode->addItem("Blackman");
    ui->FFTWindowMode->setCurrentIndex(vis_ptr->window_mode);
    ui->FFTWindowMode->blockSignals(false);

    ui->backgroundMode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->backgroundMode->addItem(visualizer_pattern_labels[i]);
    }
    ui->backgroundMode->setCurrentIndex(vis_ptr->bkgd_mode);
    ui->backgroundMode->blockSignals(false);

    ui->foregroundMode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_PATTERNS; i++)
    {
        ui->foregroundMode->addItem(visualizer_pattern_labels[i]);
    }
    ui->foregroundMode->setCurrentIndex(vis_ptr->frgd_mode);
    ui->foregroundMode->blockSignals(false);

    ui->singleColorMode->blockSignals(true);
    for(int i = 0; i < VISUALIZER_NUM_SINGLE_COLOR; i++)
    {
        ui->singleColorMode->addItem(visualizer_single_color_labels[i]);
    }
    ui->singleColorMode->setCurrentIndex(vis_ptr->single_color_mode);
    ui->singleColorMode->blockSignals(false);

    ui->averageMode->blockSignals(true);
    ui->averageMode->addItem("Binning");
    ui->averageMode->addItem("Low Pass");
    ui->averageMode->setCurrentIndex(vis_ptr->avg_mode);
    ui->averageMode->blockSignals(false);

    ui->audioDevice->blockSignals(true);
    for(unsigned int i = 0; i < vis_ptr->audio_devices.size(); i++)
    {
        ui->audioDevice->addItem(vis_ptr->audio_devices[i]);
    }
    ui->audioDevice->setCurrentIndex(vis_ptr->audio_device_idx);
    ui->audioDevice->blockSignals(false);

    ui->reactiveBackground->setChecked(vis_ptr->reactive_bkgd);
    ui->silentBackground->setChecked(vis_ptr->silent_bkgd);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(15);

    image = new QImage(256, 64, QImage::Format_RGB32);
    scene = new QGraphicsScene(this);
    ui->visualizationPreview->setScene(scene);
}

void KeyboardVisDlg::StartMinimized(boolean startmin)
{
    startminimized = startmin;
}

void KeyboardVisDlg::on_amplitude_valueChanged(const int arg1)
{
    vis_ptr->amplitude = arg1;
    vis_ptr->OnSettingsChanged();
}

void KeyboardVisDlg::on_backgroundBrightness_valueChanged(const int arg1)
{
    vis_ptr->bkgd_bright = arg1;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_averageSize_valueChanged(const int arg1)
{
    vis_ptr->avg_size = arg1;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_decay_valueChanged(const int arg1)
{
    vis_ptr->decay = arg1;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_delay_valueChanged(const int arg1)
{
    vis_ptr->delay = arg1;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_normalizationOffset_valueChanged(const double arg1)
{
    vis_ptr->nrml_ofst = arg1;
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_normalizationScale_valueChanged(const double arg1)
{
    vis_ptr->nrml_scl = arg1;
    vis_ptr->SetNormalization(vis_ptr->nrml_ofst, vis_ptr->nrml_scl);
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_animationSpeed_valueChanged(const double arg1)
{
    vis_ptr->anim_speed = arg1;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_FFTWindowMode_currentIndexChanged(int index)
{
    vis_ptr->window_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_backgroundMode_currentIndexChanged(int index)
{
    vis_ptr->bkgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_foregroundMode_currentIndexChanged(int index)
{
    vis_ptr->frgd_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_singleColorMode_currentIndexChanged(int index)
{
    vis_ptr->single_color_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_averageMode_currentIndexChanged(int index)
{
    vis_ptr->avg_mode = index;
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_pushButton_Save_Settings_clicked()
{
    vis_ptr->SaveSettings();
}

void Ui::KeyboardVisDlg::on_reactiveBackground_clicked(bool checked)
{
    vis_ptr->reactive_bkgd = checked;

    if (vis_ptr->reactive_bkgd == true)
    {
        vis_ptr->silent_bkgd = false;
        ui->silentBackground->setChecked(false);
    }
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_audioDevice_currentIndexChanged(int index)
{
    if((unsigned int)index != vis_ptr->audio_device_idx)
    {
        vis_ptr->audio_device_idx = index;
        vis_ptr->ChangeAudioDevice();
    }
}

void Ui::KeyboardVisDlg::on_filterConstant_valueChanged(const double arg1)
{
    vis_ptr->filter_constant = arg1;
    if(vis_ptr->filter_constant > 1.0f)
    {
        vis_ptr->filter_constant = 1.0f;
    }
    else if(vis_ptr->filter_constant < 0.0f)
    {
        vis_ptr->filter_constant = 0.0f;
    }
    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_silentBackground_clicked(bool checked)
{
    vis_ptr->silent_bkgd = checked;

    if (vis_ptr->silent_bkgd == true)
    {
        vis_ptr->reactive_bkgd = false;
        ui->reactiveBackground->setChecked(false);
    }

    vis_ptr->OnSettingsChanged();
}

void Ui::KeyboardVisDlg::on_backgroundTimeout_valueChanged(const int arg1)
{
    vis_ptr->background_timeout = arg1;

    if (vis_ptr->update_ui == false)
    {
        vis_ptr->background_timer = 0;
    }

    vis_ptr->OnSettingsChanged();
}
