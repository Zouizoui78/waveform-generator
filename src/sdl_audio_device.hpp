#ifndef SDL_AUDIO_DEVICE_HPP
#define SDL_AUDIO_DEVICE_HPP

#include "SDL2/SDL.h"
#include <functional>

namespace waveform_generator {

using SDLAudioCallback = std::function<void(uint8_t* raw_buffer, int len)>;

class SDLAudioDevice {
private:
    SDL_AudioDeviceID _audio_device_id = 0;
    SDLAudioCallback _sdl_callback;

    static void sdl_callback(void* raw_instance, uint8_t* raw_buffer, int len);

public:
    SDLAudioDevice(SDLAudioCallback&& sdl_callback);

    ~SDLAudioDevice() noexcept;
    SDLAudioDevice(const SDLAudioDevice& other) = delete;
    SDLAudioDevice(SDLAudioDevice&& other) = delete;
    SDLAudioDevice& operator=(const SDLAudioDevice& other) = delete;
    SDLAudioDevice& operator=(SDLAudioDevice&& other) = delete;

    SDL_AudioDeviceID get_device_id() const;
};

} // namespace waveform_generator

#endif // SDL_AUDIO_DEVICE_HPP