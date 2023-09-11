#include <iostream>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int BUFFER_SIZE = 4096;

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WIM_DATA) {
        HWAVEOUT hWaveOut = (HWAVEOUT)dwInstance;
        WAVEHDR* waveHdr = (WAVEHDR*)dwParam1;
        waveOutWrite(hWaveOut, waveHdr, sizeof(WAVEHDR));
        waveInAddBuffer(hwi, waveHdr, sizeof(WAVEHDR));
    }
}

int main() {
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    HWAVEIN hWaveIn;
    HWAVEOUT hWaveOut;

    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, (DWORD_PTR)waveInProc, (DWORD_PTR)&hWaveOut, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio input device." << std::endl;
        return 1;
    }

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio output device." << std::endl;
        waveInClose(hWaveIn);
        return 1;
    }

    WAVEHDR waveHdr;
    char buffer[BUFFER_SIZE];
    waveHdr.lpData = buffer;
    waveHdr.dwBufferLength = BUFFER_SIZE;
    waveHdr.dwFlags = 0;

    waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));

    waveInStart(hWaveIn);

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;
    std::cin.get();

    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    waveOutReset(hWaveOut);

    waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);

    return 0;
}
