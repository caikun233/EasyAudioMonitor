#include <iostream>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 音频缓冲区大小
const int BUFFER_SIZE = 4096;

int main() {
    // 打开默认的音频输入设备（麦克风）
    HWAVEIN hWaveIn;
    WAVEFORMATEX wfxIn;
    wfxIn.wFormatTag = WAVE_FORMAT_PCM;
    wfxIn.nChannels = 1;
    wfxIn.nSamplesPerSec = 48000;
    wfxIn.wBitsPerSample = 24;
    wfxIn.nBlockAlign = (wfxIn.nChannels * wfxIn.wBitsPerSample) / 8;
    wfxIn.nAvgBytesPerSec = wfxIn.nSamplesPerSec * wfxIn.nBlockAlign;
    wfxIn.cbSize = 0;

    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfxIn, 0, 0, WAVE_FORMAT_DIRECT) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio input device." << std::endl;
        return 1;
    } else {
        std::cout << "Audio input device opened successfully." << std::endl;
    }

    // 打开默认的音频输出设备（扬声器）
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfxOut;
    wfxOut.wFormatTag = WAVE_FORMAT_PCM;
    wfxOut.nChannels = 2;
    wfxOut.nSamplesPerSec = 48000;
    wfxOut.wBitsPerSample = 16;
    wfxOut.nBlockAlign = (wfxOut.nChannels * wfxOut.wBitsPerSample) / 8;
    wfxOut.nAvgBytesPerSec = wfxOut.nSamplesPerSec * wfxOut.nBlockAlign;
    wfxOut.cbSize = 0;

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfxOut, 0, 0, WAVE_FORMAT_DIRECT) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio output device." << std::endl;
        waveInClose(hWaveIn);
        return 1;
    } else {
        std::cout << "Audio output device opened successfully." << std::endl;
    }

    // 初始化音频输入缓冲区
    WAVEHDR waveHdr;
    waveHdr.lpData = (LPSTR)malloc(BUFFER_SIZE);
    waveHdr.dwBufferLength = BUFFER_SIZE;
    waveHdr.dwBytesRecorded = 0;
    waveHdr.dwUser = 0;
    waveHdr.dwFlags = 0;
    waveHdr.dwLoops = 0;
    waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));

    // 开始音频输入和输出
    waveInStart(hWaveIn);
    MMRESULT result = waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to write audio output. Error code: " << result << std::endl;
    }

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;
    std::cin.get();

    // 停止音频输入和输出
    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    waveOutReset(hWaveOut);

    // 关闭设备并释放资源
    waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);
    free(waveHdr.lpData);

    return 0;
}
