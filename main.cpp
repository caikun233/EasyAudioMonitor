#include <iostream>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int BUFFER_SIZE = 4096;

// 回调函数，用于处理音频数据
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WIM_DATA) {
        WAVEHDR* waveHdr = (WAVEHDR*)dwParam1;
        HWAVEOUT hWaveOut = (HWAVEOUT)dwInstance;

        // 将音频数据写入输出设备
        waveOutWrite(hWaveOut, waveHdr, sizeof(WAVEHDR));
        
        // 重新准备音频输入缓冲区
        waveInAddBuffer(hwi, waveHdr, sizeof(WAVEHDR));
    }
}

int main() {
    HWAVEIN hWaveIn;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;

    // 音频格式设置
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    // 打开音频输入和输出设备
    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, (DWORD_PTR)waveInProc, (DWORD_PTR)&hWaveOut, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio input device." << std::endl;
        return 1;
    }

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio output device." << std::endl;
        waveInClose(hWaveIn);
        return 1;
    }

    // 音频缓冲区设置
    WAVEHDR waveHdr;
    waveHdr.lpData = (LPSTR)malloc(BUFFER_SIZE);
    waveHdr.dwBufferLength = BUFFER_SIZE;
    waveHdr.dwFlags = 0;

    waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));

    // 开始音频捕获和播放
    waveInStart(hWaveIn);

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;
    std::cin.get();

    // 停止并清理
    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    waveOutReset(hWaveOut);

    waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);
    free(waveHdr.lpData);

    return 0;
}
