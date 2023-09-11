#include <iostream>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

const int BUFFER_SIZE = 4096;

void checkError(MMRESULT result, const char* errorMessage) {
    if (result != MMSYSERR_NOERROR) {
        char errorText[128];
        waveInGetErrorText(result, errorText, sizeof(errorText));
        std::cerr << errorMessage << " Error code: " << result << " - " << errorText << std::endl;
    }
}

int main() {
    HWAVEIN hWaveIn;
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, 0, 0, WAVE_FORMAT_DIRECT);
    checkError(result, "Failed to open audio input device.");
    if (result != MMSYSERR_NOERROR) return 1;

    std::cout << "Audio input device opened successfully." << std::endl;

    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfxOut = wfx;
    wfxOut.nChannels = 2;
    wfxOut.wBitsPerSample = 16;
    wfxOut.nBlockAlign = (wfxOut.nChannels * wfxOut.wBitsPerSample) / 8;
    wfxOut.nAvgBytesPerSec = wfxOut.nSamplesPerSec * wfxOut.nBlockAlign;

    result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfxOut, 0, 0, WAVE_FORMAT_DIRECT);
    checkError(result, "Failed to open audio output device.");
    if (result != MMSYSERR_NOERROR) {
        waveInClose(hWaveIn);
        return 1;
    }

    std::cout << "Audio output device opened successfully." << std::endl;

    WAVEHDR waveHdr;
    waveHdr.lpData = (LPSTR)malloc(BUFFER_SIZE);
    waveHdr.dwBufferLength = BUFFER_SIZE;
    waveHdr.dwBytesRecorded = 0;
    waveHdr.dwUser = 0;
    waveHdr.dwFlags = 0;
    waveHdr.dwLoops = 0;

    waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));

    waveInStart(hWaveIn);

    std::cout << "Listening to audio (Press Enter to stop)..." << std::endl;

    while (true) {
        if (waveHdr.dwFlags & WHDR_DONE) {
            std::cout << "Received audio data. Writing to output..." << std::endl;
            result = waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR));
            checkError(result, "Failed to write audio output.");
            waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));
        }

        if (GetAsyncKeyState(VK_RETURN)) break;
    }

    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    waveOutReset(hWaveOut);

    waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);
    free(waveHdr.lpData);

    return 0;
}
