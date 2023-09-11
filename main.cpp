#include <iostream>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int BUFFER_SIZE = 4096;
const int NUM_BUFFERS = 3;

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WIM_DATA) {
        std::cout << "Received audio data. Writing to output..." << std::endl;
        MMRESULT result = waveOutWrite((HWAVEOUT)dwInstance, (WAVEHDR*)dwParam1, sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Failed to write audio output. Error code: " << result << std::endl;
        }
    }
}

int main() {
    HWAVEIN hWaveIn;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfxIn, wfxOut;
    WAVEHDR waveHdr[NUM_BUFFERS];

    // Input format
    wfxIn.wFormatTag = WAVE_FORMAT_PCM;
    wfxIn.nChannels = 1;
    wfxIn.nSamplesPerSec = 48000;
    wfxIn.wBitsPerSample = 16;
    wfxIn.nBlockAlign = (wfxIn.nChannels * wfxIn.wBitsPerSample) / 8;
    wfxIn.nAvgBytesPerSec = wfxIn.nSamplesPerSec * wfxIn.nBlockAlign;
    wfxIn.cbSize = 0;

    // Output format
    wfxOut.wFormatTag = WAVE_FORMAT_PCM;
    wfxOut.nChannels = 1;
    wfxOut.nSamplesPerSec = 48000;
    wfxOut.wBitsPerSample = 16;
    wfxOut.nBlockAlign = (wfxOut.nChannels * wfxOut.wBitsPerSample) / 8;
    wfxOut.nAvgBytesPerSec = wfxOut.nSamplesPerSec * wfxOut.nBlockAlign;
    wfxOut.cbSize = 0;

    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfxIn, (DWORD_PTR)waveInProc, (DWORD_PTR)&hWaveOut, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio input device." << std::endl;
        return 1;
    }
    std::cout << "Audio input device opened successfully." << std::endl;

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfxOut, 0, 0, WAVE_FORMAT_DIRECT) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open audio output device." << std::endl;
        waveInClose(hWaveIn);
        return 1;
    }
    std::cout << "Audio output device opened successfully." << std::endl;

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        waveHdr[i].lpData = (LPSTR)malloc(BUFFER_SIZE);
        waveHdr[i].dwBufferLength = BUFFER_SIZE;
        waveHdr[i].dwFlags = 0;
        waveInPrepareHeader(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHdr[i], sizeof(WAVEHDR));
    }

    waveInStart(hWaveIn);

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
