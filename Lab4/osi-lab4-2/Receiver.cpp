#include <fstream>
#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;


void CreateSenderProcesses(const string& file_name, int number_of_senders, HANDLE* hEventStarted);

void HandleMessages(const string& file_name, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex);

int main() {
    string file_name;
    int count_notes;
    fstream file;
    int count_senders;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    cout << " Write name of the file : ";
    cin >> file_name;
    cout << " Write how many notice do you want to make: ";
    cin >> count_notes;
    file.open(file_name, std::ios::out);
    file.close();
    cout << " Write  how many Sender processes do you need: ";
    cin >> count_senders;

    HANDLE hInputReadySemaphore = CreateSemaphore(NULL, 0, count_notes, "EnterSemaphoreStarted");
    if (hInputReadySemaphore == NULL)
        return GetLastError();
    HANDLE hOutputReadySemaphore = CreateSemaphore(NULL, 0, count_notes, "OutputSemaphoreStarted");
    if (hOutputReadySemaphore == NULL)
        return GetLastError();
    HANDLE hMutex = CreateMutex(NULL, 0, "mut ex");
    HANDLE* hEventStarted = new HANDLE[count_senders];

    CreateSenderProcesses(file_name, count_senders, hEventStarted);

    WaitForMultipleObjects(count_senders, hEventStarted, TRUE, INFINITE);
    ReleaseMutex(hMutex);

    HandleMessages(file_name, hInputReadySemaphore, hOutputReadySemaphore, hMutex);

    CloseHandle(hInputReadySemaphore);
    CloseHandle(hOutputReadySemaphore);
    for (int i = 0; i < count_senders; i++) {
        CloseHandle(hEventStarted[i]);
    }

    delete[] hEventStarted;

    return 0;
}

void CreateSenderProcesses(const string& file_name, int count_senders, HANDLE* hEventStarted) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    for (int i = 0; i < count_senders; ++i) {
        string sender_cmd = "Sender.exe " + file_name;
        LPSTR lpwstrSenderProcessCommandLine = const_cast<LPSTR>(sender_cmd.c_str());

        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, lpwstrSenderProcessCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            cout << "You can't start Sender Process"<<endl;
            exit(GetLastError());
        }
        hEventStarted[i] = CreateEvent(NULL, FALSE, FALSE, "StartPocess");
        if (hEventStarted[i] == NULL)
            exit(GetLastError());
        CloseHandle(pi.hProcess);
    }
}

void HandleMessages(const string& file_name, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex) {
    fstream file;
    cout<<"\nEnter 'read' to read message"<<endl;
    cout<<"Enter 'exit' to exit "<<endl;
    string flag;
    cin >> flag;
    file.open(file_name, std::ios::in);

    while (true) {
        if (flag == "read") {
            string message;
            WaitForSingleObject(hInputReadySemaphore, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);
            getline(file, message);
            cout << message;
            ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);
            ReleaseMutex(hMutex);
            cout << "\nEnter 'read' to read message;" << endl;
            cout<<"Enter 'exit' to exit"<<endl;
            cin >> flag;
        }
         else if(flag == "exit") {
            cout << "All processes finished their work" << endl;
            break;
        }

         else{
            cout<<"\nSorry, but it is a wrong value"<<endl;
            cout<<"Enter 'read' to read message;" << endl;
            cout<<"Enter 'exit' to exit"<<endl;
            cin >> flag;
        }
        
    }
    file.close();
}