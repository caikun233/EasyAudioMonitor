#include <iostream>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int BUFFER_SIZE = 4096;
const int NUM_BUFFERS = 2;

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WOM_DONE) {
        WAVEHDR* waveHdr = (WAVEHDR*)dwParam1;
        MMRESULT result = waveOutWrite(hwo, waveHdr, sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Failed to write audio output. Error code: " << result << std::endl;
        } else {
            std::cout << "Audio output written successfully." << std::endl;
        }
    }
}

int main() {
    HWAVEIN hWaveIn;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    WAVEHDR waveHdr[NUM_BUFFERS];

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio input device. Error code: " << result << std::endl;
        return 1;
    }

    result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio output device. Error code: " << result << std::endl;
        waveInClose(hWaveIn);
        return 1;
    }

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        waveHdr[i].lpData = (LPSTR)malloc(BUFFER_SIZE);
        waveHdr[i].dwBufferLength = BUFFER_SIZE;
        waveHdr[i].dwBytesRecorded = 0;
        waveHdr[i].dwUser = 0;
        waveHdr[i].dwFlags = 0;
        waveHdr[i].dwLoops = 0;
        waveInPrepareHeader(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
    }

    result = waveInStart(hWaveIn);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to start audio input. Error code: " << result << std::endl;
        return 1;
    }

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;
    std::cin.get();

    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    waveOutReset(hWaveOut);

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        waveInUnprepareHeader(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
        free(waveHdr[i].lpData);
    }

    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);

    return 0;
}
