#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
    auto sinus = std::make_shared<tools::sdl::Sinus>();
    _sound_player.add_sound(sinus);
}

MainWindow::~MainWindow() {}

void MainWindow::on_play_pause_pushButton_clicked() {
    if (_playing) {
        _sound_player.pause();
        _ui->play_pause_pushButton->setText("Play");
    }
    else {
        _sound_player.play();
        _ui->play_pause_pushButton->setText("Pause");
    }
    _playing = !_playing;
}