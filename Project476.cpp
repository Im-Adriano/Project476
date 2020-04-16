// Project476.cpp : Defines the entry point for the application.
//

#include "Project476.h"
#include "command_socket.h"

using namespace std;

int main()
{
	recieve_commands();
	return 0;
}

HANDLE thr;

extern "C" __declspec(dllexport) DWORD WINAPI StartMain(LPVOID data) {
    recieve_commands();
    return 0;
}

__declspec(dllexport) BOOL WINAPI DllMain(IN HINSTANCE hModule,
    IN DWORD  ul_reason_for_call,
    IN LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, &StartMain, (LPVOID)NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
