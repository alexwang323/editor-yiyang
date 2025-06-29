// imeManager.cpp
#include "ginkgopch.h"
#include "imeManager.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

void DisableIME(HWND hwnd) {
    HIMC hIMC = ImmGetContext(hwnd);
    if (hIMC) {
        ImmAssociateContext(hwnd, NULL);
        ImmReleaseContext(hwnd, hIMC);
    }
}

void EnableIME(HWND hwnd) {
    HIMC hIMC = ImmGetContext(hwnd);
    if (hIMC == NULL) {
        HIMC newIMC = ImmCreateContext();
        ImmAssociateContext(hwnd, newIMC);
    }
    else {
        ImmReleaseContext(hwnd, hIMC);
    }
}
