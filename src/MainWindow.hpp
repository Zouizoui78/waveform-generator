#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "QChart"
#include "QMainWindow"
#include "QValueAxis"

#include "waveform_player.hpp"
#include "tools/waveform/waveform_generator.hpp"
#include "tools/waveform/waveform_base.hpp"

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
    void update_harmonics_groupbox();
    void update_ui();

private:
    std::unique_ptr<Ui::MainWindow> _ui;

    waveform_generator::WaveformPlayer _sound_player;
    std::vector<std::unique_ptr<tools::waveform::WaveformBase>> _waveforms;

    QChart* _time_chart;
    QChart* _freq_chart;
    QChart* _time_details_chart;

    void init_charts();
    void update_time_chart(const std::vector<double>& samples);
    void update_freq_chart(const std::vector<double>& samples);
    void update_time_details_chart();
    QList<QPointF> samples_to_point_list(
        std::vector<double>::const_iterator begin,
        std::vector<double>::const_iterator end
    ) const;

    // Hide legend, create default axes and set axes names.
    // Return a pair containing the two axes.
    std::pair<QValueAxis *, QValueAxis *>
    set_chart_defaults(
        QChart *chart,
        const std::string &x_axis_name,
        const std::string &y_axis_name
    );

    std::vector<double> fft(const std::vector<double>& samples) const;

};

#endif // MAINWINDOW_HPP
