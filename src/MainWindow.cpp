#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "QChart"
#include "QChartView"
#include "QLineSeries"

#include "kissfft/kiss_fftr.h"
#include "spdlog/spdlog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(std::make_unique<Ui::MainWindow>()),
    _waveform_generator(std::make_shared<tools::waveform::WaveformGenerator>()),
    _sound_player(_waveform_generator)
{
    _ui->setupUi(this);

    for (int i = 1 ; i <= 10 ; i++) {
        int j = i * 2 - 1;
        // spdlog::info("i = {} ; j = {}", i, j);
        auto sin = std::make_shared<tools::waveform::Sinus>();
        sin->set_frequency(440 * j);
        sin->set_volume(1.0 / j);
        _waveform_generator->add_waveform(sin);
    }

    init_chart();
}

MainWindow::~MainWindow() {}

void MainWindow::on_play_pause_pushButton_clicked() {
    if (_sound_player.is_playing()) {
        _sound_player.pause();
        _ui->play_pause_pushButton->setText("Play");
    }
    else {
        _sound_player.play();
        _ui->play_pause_pushButton->setText("Pause");
    }
}

void MainWindow::init_chart() {
    auto samples = _waveform_generator->generate_n_samples(100);
    auto fft_output = fft(samples);

    auto series = new QLineSeries;
    for (int i = 0 ; i < samples.size() ; i++) {
        series->append(i, samples[i]);
    }

    auto series2 = new QLineSeries;
    for (int i = 0 ; i < fft_output.size() ; i++) {
        double freq = 440 * i;
        series2->append(freq, fft_output[i]);
    }

    auto chart = new QChart;
    chart->addSeries(series);
    chart->legend()->hide();
    chart->createDefaultAxes();

    auto chart_view = new QChartView(chart, this);
    chart_view->setRenderHint(QPainter::Antialiasing);

    auto chart2 = new QChart;
    chart2->addSeries(series2);
    chart2->legend()->hide();
    chart2->createDefaultAxes();

    auto chart_view2 = new QChartView(chart2, this);
    chart_view2->setRenderHint(QPainter::Antialiasing);

    _ui->charts_layout->addWidget(chart_view);
    _ui->charts_layout->addWidget(chart_view2);
}

std::vector<double> MainWindow::fft(std::vector<double> samples) {
    kiss_fftr_cfg cfg = kiss_fftr_alloc(samples.size(), 0, nullptr, nullptr);
    std::vector<float> samples_float;
    for (auto sample : samples) {
        samples_float.push_back(sample);
    }

    size_t size = samples.size() / 2 + 1;
    kiss_fft_cpx *fft_output = new kiss_fft_cpx[size];

    kiss_fftr(cfg, samples_float.data(), fft_output);

    std::vector<double> ret;
    for (int i = 0 ; i < size ; i++) {
        spdlog::info("{}, {}", fft_output[i].r, fft_output[i].i);
        ret.push_back(sqrt(pow(fft_output[i].r, 2) + pow(fft_output[i].i, 2)));
    }

    free(cfg);
    delete[] fft_output;
    return ret;
}
