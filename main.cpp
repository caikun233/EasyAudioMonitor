#include <iostream>
#include <vector>
#include <portaudio.h>

// 定义麦克风和耳机的参数
const int microphoneDevice = 1;  // 选择正确的麦克风设备（可以使用PortAudio的Pa_GetDeviceInfo函数来查看可用设备）
const int headphoneDevice = 2;   // 选择正确的耳机设备

const int sampleRate = 48000;   // 采样率（可以根据需要进行调整）
const int bufferSize = 512;    // 缓冲区大小（可以根据需要进行调整）

// 回调函数来处理音频数据
static int audioCallback(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData) {
    float *in = (float *)inputBuffer;
    float *out = (float *)outputBuffer;
    (void)timeInfo;  // 未使用的参数

    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        *out++ = *in++;  // 将麦克风输入数据直接传输到耳机
    }

    return paContinue;
}

int main() {
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, sampleRate, bufferSize, audioCallback, nullptr);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    std::cout << "实时监听中，请按Ctrl+C来停止..." << std::endl;
    while (true) {
        Pa_Sleep(1000);  // 在此处休眠以允许继续监听
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    Pa_Terminate();

    return 0;
}
