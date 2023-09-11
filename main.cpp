#include <iostream>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 音频缓冲区大小
const int BUFFER_SIZE = 4096;

void checkMMResult(MMRESULT result, const char* errorMsg) {
    if (result != MMSYSERR_NOERROR) {
        std::cerr << errorMsg << " Error code: " << result << std::endl;
    }
}

int main() {
    // 打开默认的音频输入设备（麦克风）
    HWAVEIN hWaveIn;
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;                // 立体声输入
    wfx.nSamplesPerSec = 44100;       // 采样率
    wfx.wBitsPerSample = 16;          // 16位每样本
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT);
    checkMMResult(result, "Failed to open audio input device.");

    // 打开默认的音频输出设备（扬声器）
    HWAVEOUT hWaveOut;
    result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT);
    checkMMResult(result, "Failed to open audio output device.");

    // 初始化音频输入缓冲区
    WAVEHDR waveHdr;
    waveHdr.lpData = (LPSTR)malloc(BUFFER_SIZE);
    waveHdr.dwBufferLength = BUFFER_SIZE;
    waveHdr.dwBytesRecorded = 0;
    waveHdr.dwUser = 0;
    waveHdr.dwFlags = 0;
    waveHdr.dwLoops = 0;

    result = waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    checkMMResult(result, "Failed to prepare header for audio input device.");

    result = waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    checkMMResult(result, "Failed to add buffer to audio input device.");

    // 开始音频输入和输出
    result = waveInStart(hWaveIn);
    checkMMResult(result, "Failed to start audio input.");

    result = waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    checkMMResult(result, "Failed to write audio output.");

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;
    std::cin.get();

    // 停止音频输入和输出
    result = waveInStop(hWaveIn);
    checkMMResult(result, "Failed to stop audio input.");

    result = waveInReset(hWaveIn);
    checkMMResult(result, "Failed to reset audio input.");

    result = waveOutReset(hWaveOut);
    checkMMResult(result, "Failed to reset audio output.");

    // 关闭设备并释放资源
    result = waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    checkMMResult(result, "Failed to unprepare header for audio input device.");

    result = waveInClose(hWaveIn);
    checkMMResult(result, "Failed to close audio input device.");

    result = waveOutClose(hWaveOut);
    checkMMResult(result, "Failed to close audio output device.");

    free(waveHdr.lpData);

    return 0;
}
