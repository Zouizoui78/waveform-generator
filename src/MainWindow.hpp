#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "QMainWindow"
#include "tools/sdl/Sound.hpp"
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
    tools::sdl::SoundPlayer _sound_player;
    bool _playing = false;
};

#endif // MAINWINDOW_HPP