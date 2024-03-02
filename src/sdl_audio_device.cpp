#include "sdl_audio_device.hpp"
#include "tools/waveform/constants.hpp"

#include <stdexcept>
#include <utility>

namespace waveform_generator {

SDLAudioDevice::~SDLAudioDevice() {
    if (_audio_device_id != 0) {
        SDL_CloseAudioDevice(_audio_device_id);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

SDLAudioDevice::SDLAudioDevice(SDLAudioCallback&& sdl_callback)
    : _sdl_callback(sdl_callback) {
    if (!_sdl_callback) {
        throw std::runtime_error("SDLAudioDevice : Invalid callback");
    }

#ifdef WIN32
    putenv("SDL_AUDIODRIVER=dsound");
#endif
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_AudioSpec desired;
    desired.freq = tools::waveform::constants::sampling_rate;
    desired.format = AUDIO_F32SYS;
    desired.channels = 1;
    desired.samples = 512;
    desired.callback =
        &SDLAudioDevice::sdl_callback; // called periodically by SDL to refill
                                       // the buffer
    desired.userdata = this;

    _audio_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);

    if (_audio_device_id == 0) {
        throw std::runtime_error(SDL_GetError());
    }
}

SDL_AudioDeviceID SDLAudioDevice::get_device_id() const {
    return _audio_device_id;
}

void SDLAudioDevice::sdl_callback(void* raw_instance, uint8_t* raw_buffer,
                                  int len) {
    SDLAudioDevice* instance = static_cast<SDLAudioDevice*>(raw_instance);
    instance->_sdl_callback(raw_buffer, len);
}

} // namespace waveform_generator