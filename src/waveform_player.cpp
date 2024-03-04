#include "waveform_player.hpp"
#include "tools/waveform/constants.hpp"

namespace waveform_generator {

WaveformPlayer::WaveformPlayer()
    : _audio_device(std::make_unique<SDLAudioDevice>(
          std::bind_front(&WaveformPlayer::sdl_callback, this))) {}

bool WaveformPlayer::add_waveform(
    const tools::waveform::WaveformBase* waveform) {
    return _waveform_generator.add_waveform(waveform);
}

bool WaveformPlayer::remove_waveform(
    const tools::waveform::WaveformBase* waveform) {
    return _waveform_generator.remove_waveform(waveform);
}

void WaveformPlayer::play() const {
    SDL_PauseAudioDevice(_audio_device->get_device_id(), 0);
}

void WaveformPlayer::pause() const {
    SDL_PauseAudioDevice(_audio_device->get_device_id(), 1);
}

bool WaveformPlayer::is_paused() const {
    return SDL_GetAudioDeviceStatus(_audio_device->get_device_id()) ==
           SDL_AUDIO_PAUSED;
}

void WaveformPlayer::sdl_callback(uint8_t* raw_buffer, int len) {
    float* buffer = reinterpret_cast<float*>(raw_buffer);
    len /= sizeof(float);
    for (int i = 0; i < len; i++) {
        buffer[i] = static_cast<float>(_waveform_generator.generate_sample());
    }
}

tools::waveform::WaveformGenerator& WaveformPlayer::get_generator() {
    return _waveform_generator;
}

} // namespace waveform_generator
