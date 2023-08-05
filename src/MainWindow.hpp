#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "QMainWindow"

#include "tools/sdl/WaveformPlayer.hpp"
#include "tools/waveform/WaveformGenerator.hpp"
#include "tools/waveform/Waveforms.hpp"

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

private:
    std::unique_ptr<Ui::MainWindow> _ui;

    tools::sdl::WaveformPlayer _sound_player;
    std::shared_ptr<tools::waveform::WaveformGenerator> _waveform_generator;

    void init_chart();

    std::vector<double> fft(std::vector<double> samples);
};

#endif // MAINWINDOW_HPP
