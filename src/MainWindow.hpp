#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "QChart"
#include "QMainWindow"

#include "tools/sdl/WaveformPlayer.hpp"
#include "tools/waveform/WaveformGenerator.hpp"
#include "tools/waveform/AWaveform.hpp"

#undef main

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void on_play_pause_pushButton_clicked();
    void on_add_harmonic_pushButton_clicked();
    void on_remove_harmonic_pushButton_clicked();

    void update_charts();
    void update_harmonics_buttons();
    void update_ui();

private:
    std::unique_ptr<Ui::MainWindow> _ui;

    std::shared_ptr<tools::waveform::WaveformGenerator> _waveform_generator;
    tools::sdl::WaveformPlayer _sound_player;
    std::vector<std::shared_ptr<tools::waveform::AWaveform>> _waveforms;

    QChart* _time_chart;
    QChart* _freq_chart;

    void init_charts();

    std::vector<double> fft(std::vector<double> samples);
};

#endif // MAINWINDOW_HPP
