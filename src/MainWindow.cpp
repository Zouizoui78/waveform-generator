#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "QChartView"
#include "QLineSeries"

#include "kissfft/kiss_fftr.h"
#include "spdlog/spdlog.h"

#include "tools/waveform/Constants.hpp"
#include "tools/waveform/Waveforms.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(std::make_unique<Ui::MainWindow>()),
    _waveform_generator(std::make_shared<tools::waveform::WaveformGenerator>()),
    _sound_player(_waveform_generator)
{
    _ui->setupUi(this);

    _waveforms.emplace_back(std::make_shared<tools::waveform::Sinus>());
    _waveform_generator->add_waveform(_waveforms.back());

    init_charts();
    update_ui();
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

void MainWindow::on_add_harmonic_pushButton_clicked() {
    auto waveform = std::make_shared<tools::waveform::Sinus>();
    waveform->set_frequency(440.0 * (2 * _waveforms.size() + 1));
    waveform->set_volume(1.0 / (2 * _waveforms.size() + 1));

    _waveforms.push_back(waveform);
    _waveform_generator->add_waveform(_waveforms.back());

    update_ui();
}

void MainWindow::on_remove_harmonic_pushButton_clicked() {
    _waveform_generator->remove_waveform(_waveforms.back());
    _waveforms.pop_back();

    update_ui();
}

void MainWindow::update_charts() {
    int n_points = 
        tools::waveform::sampling_rate /
        _waveforms.front()->get_frequency();

    bool was_playing = _sound_player.is_playing();
    if (was_playing) {
        _sound_player.pause();
    }

    _waveform_generator->reset_samples();
    auto samples = _waveform_generator->generate_n_samples(n_points);

    if (was_playing) {
        _sound_player.play();
    }

    update_time_chart(samples);
    update_freq_chart(samples);
    update_time_details_chart();
}

void MainWindow::update_harmonics_groupbox() {
    _ui->add_harmonic_pushButton->setDisabled(_waveforms.size() >= 10);
    _ui->remove_harmonic_pushButton->setDisabled(_waveforms.size() <= 1);
    _ui->harmonics_count_label->setText(QString::number(_waveforms.size()));
}

void MainWindow::update_ui() {
    update_charts();
    update_harmonics_groupbox();
}

void MainWindow::init_charts() {
    _time_chart = new QChart;
    _time_details_chart = new QChart;
    _freq_chart = new QChart;

    auto chart_view = new QChartView(_time_chart, this);
    chart_view->setRenderHint(QPainter::Antialiasing);

    auto time_detail_chart_view = new QChartView(_time_details_chart, this);
    time_detail_chart_view->setRenderHint(QPainter::Antialiasing);

    auto chart_view2 = new QChartView(_freq_chart, this);
    chart_view2->setRenderHint(QPainter::Antialiasing);

    _ui->time_charts_layout->addWidget(chart_view);
    _ui->time_charts_layout->addWidget(time_detail_chart_view);
    _ui->charts_layout->addWidget(chart_view2);
}

void MainWindow::update_time_chart(std::vector<double> samples) {
    _time_chart->removeAllSeries();
    auto series = new QLineSeries;
    series->append(samples_to_point_list(samples));
    
    _time_chart->addSeries(series);
    _time_chart->legend()->hide();
    _time_chart->createDefaultAxes();
    _time_chart->axes()[0]->setMin(0);
    _time_chart->axes()[0]->setMax(1.0 / _waveforms.front()->get_frequency());
}

void MainWindow::update_freq_chart(std::vector<double> samples) {
    _freq_chart->removeAllSeries();
    auto fft_output = fft(samples);

    auto series = new QLineSeries;
    for (int i = 0 ; i < fft_output.size() ; i++) {
        double freq = 440 * i;
        series->append(freq, fft_output[i]);
    }

    _freq_chart->addSeries(series);
    _freq_chart->legend()->hide();
    _freq_chart->createDefaultAxes();
}

void MainWindow::update_time_details_chart() {
    _time_details_chart->removeAllSeries();
    for (const auto& sound : _waveforms) {
        tools::waveform::WaveformGenerator g;
        g.add_waveform(sound);
        auto samples { g.generate_n_samples(100) };
        auto points { samples_to_point_list(samples) };
        auto series = new QLineSeries;
        series->append(points);
        _time_details_chart->addSeries(series);
    }
    _time_details_chart->legend()->hide();
    _time_details_chart->createDefaultAxes();
}

QList<QPointF> MainWindow::samples_to_point_list(std::vector<double> samples) {
    QList<QPointF> points;
    points.reserve(samples.size());
    for (int i = 0 ; i < samples.size() ; i++) {
        points.emplace_back(
            static_cast<double>(i) / static_cast<double>(tools::waveform::sampling_rate),
            samples[i]
        );
    }
    return points;
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
        ret.push_back(sqrt(pow(fft_output[i].r, 2) + pow(fft_output[i].i, 2)));
    }

    free(cfg);
    delete[] fft_output;
    return ret;
}
