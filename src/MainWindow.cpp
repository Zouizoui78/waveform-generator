#include "MainWindow.hpp"
#include "kissfft_wrapper.hpp"
#include "ui_MainWindow.h"

#include "QChartView"
#include "QLineSeries"

#include "tools/waveform/constants.hpp"
#include "tools/waveform/waveforms.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), _ui(std::make_unique<Ui::MainWindow>()),
      _sound_player() {
    _ui->setupUi(this);

    _waveforms.emplace_back(std::make_unique<tools::waveform::Sinus>());
    _waveforms.back().get()->set_frequency(440);
    _sound_player.add_waveform(_waveforms.back().get());

    init_charts();
    update_ui();
}

MainWindow::~MainWindow() {}

void MainWindow::on_play_pause_pushButton_clicked() {
    if (_sound_player.is_paused()) {
        play();
    } else {
        pause();
    }
}

void MainWindow::on_add_harmonic_pushButton_clicked() {
    _waveforms.emplace_back(std::make_unique<tools::waveform::Sinus>());
    auto waveform = _waveforms.back().get();

    int harmoc_index = _waveforms.size() - 1;
    int mode = 2 * _waveforms.size() - 1;
    double fundamental = _waveforms.front()->get_frequency();
    waveform->set_frequency(fundamental * mode);
    waveform->set_volume(1.0 / mode);
    // waveform->set_volume(pow(-1.0, harmoc_index) * pow(mode, -2.0));

    _sound_player.get_generator().add_waveform(_waveforms.back().get());

    update_ui();
}

void MainWindow::on_remove_harmonic_pushButton_clicked() {
    _sound_player.get_generator().remove_waveform(_waveforms.back().get());
    _waveforms.pop_back();

    update_ui();
}

void MainWindow::update_charts() {
    bool was_playing = !_sound_player.is_paused();
    if (was_playing) {
        _sound_player.pause();
    }

    int64_t sample_index = _sound_player.get_generator().get_sample_index();
    _sound_player.get_generator().reset_sample_index();
    auto samples = _sound_player.get_generator().generate_n_samples(10000);
    _sound_player.get_generator().set_sample_index(sample_index);

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

void MainWindow::play() {
    _sound_player.play();
    _ui->play_pause_pushButton->setText("Pause");
}

void MainWindow::pause() {
    _sound_player.pause();
    _sound_player.get_generator().reset_sample_index();
    _ui->play_pause_pushButton->setText("Play");
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

void MainWindow::update_time_chart(const std::vector<double>& samples) {
    int n_points = tools::waveform::constants::sampling_rate /
                   _waveforms.front()->get_frequency() * 2;

    _time_chart->removeAllSeries();
    auto series = new QLineSeries;
    series->append(
        samples_to_point_list(samples.begin(), samples.begin() + n_points));

    _time_chart->addSeries(series);
    auto axes{set_chart_defaults(_time_chart, "Time (s)", "Volume")};
    axes.first->setMax(1.0 / _waveforms.front()->get_frequency() * 2);
}

void MainWindow::update_freq_chart(const std::vector<double>& samples) {
    _freq_chart->removeAllSeries();
    auto fft_output = kissfft::fft(samples);

    auto series = new QLineSeries;
    double freq_mult =
        static_cast<double>(tools::waveform::constants::sampling_rate) /
        samples.size();
    for (int i = 0; i < fft_output.size(); i++) {
        double freq = i * freq_mult;
        series->append(freq, fft_output[i]);
    }

    _freq_chart->addSeries(series);
    auto axes{set_chart_defaults(_freq_chart, "Frequency (Hz)", "Volume")};
    axes.first->setTickType(QValueAxis::TickType::TicksDynamic);
    axes.first->setTickInterval(_waveforms.front()->get_frequency());
    axes.first->setMax(20 * _waveforms.front()->get_frequency());
}

void MainWindow::update_time_details_chart() {
    int n_points = tools::waveform::constants::sampling_rate /
                   _waveforms.front()->get_frequency();

    _time_details_chart->removeAllSeries();
    for (const auto& sound : _waveforms) {
        tools::waveform::WaveformGenerator g;
        g.add_waveform(sound.get());
        auto samples{g.generate_n_samples(n_points)};
        auto points{samples_to_point_list(samples.begin(), samples.end())};
        auto series = new QLineSeries;
        series->append(points);
        _time_details_chart->addSeries(series);
    }
    set_chart_defaults(_time_details_chart, "Time (s)", "Volume");
}

QList<QPointF> MainWindow::samples_to_point_list(
    std::vector<double>::const_iterator begin,
    std::vector<double>::const_iterator end) const {
    QList<QPointF> points;
    auto size{end - begin};
    points.reserve(size);
    for (int i = 0; i < size; i++) {
        points.emplace_back(
            static_cast<double>(i) /
                static_cast<double>(tools::waveform::constants::sampling_rate),
            *(begin + i));
    }
    return points;
}

std::pair<QValueAxis*, QValueAxis*>
MainWindow::set_chart_defaults(QChart* chart, const std::string& x_axis_name,
                               const std::string& y_axis_name) {
    chart->legend()->hide();
    chart->createDefaultAxes();

    auto axes{chart->axes()};
    QValueAxis* x = static_cast<QValueAxis*>(chart->axes()[0]);
    QValueAxis* y = static_cast<QValueAxis*>(chart->axes()[1]);

    x->setTitleText(QString::fromStdString(x_axis_name));
    y->setTitleText(QString::fromStdString(y_axis_name));

    std::pair<QValueAxis*, QValueAxis*> ret{x, y};
    return ret;
}