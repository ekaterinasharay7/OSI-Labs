#include <fstream>
#include <Windows.h>
#include <conio.h>
#include <iostream>
using namespace std;


void processMessages(const std::string& file_name, HANDLE hStartEvent, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file_name>\n";
        return 1;
    }

    string file_name = argv[1];
    fstream file;
    HANDLE hStartEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "StartPocess");

    if (hStartEvent == NULL) {
        cout<<"\nYou haven't open event"<<endl;
        cout<<"Press something to exit to exit"<<endl;
        cin.get();
        return GetLastError();
    }

    HANDLE hInputReadySemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, "EnterSemaphoreStarted");
    if (hInputReadySemaphore == NULL)
        return GetLastError();

    HANDLE hOutputReadySemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, "OutputSemaphoreStarted");
    if (hOutputReadySemaphore == NULL)
        return GetLastError();

    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, "mut ex");

    SetEvent(hStartEvent);
    cout << "Event was started\n";

    processMessages(file_name, hStartEvent, hInputReadySemaphore, hOutputReadySemaphore, hMutex);

    CloseHandle(hInputReadySemaphore);
    CloseHandle(hOutputReadySemaphore);
    CloseHandle(hMutex);

    return 0;
}

void processMessages(const std::string& file_name, HANDLE hStartEvent, HANDLE hInputReadySemaphore, HANDLE hOutputReadySemaphore, HANDLE hMutex) {
    fstream file;
    cout <<"\nEnter 'write' to write message;"<<endl;
    cout<<"Enter 'exit' to exit process"<<endl;
    string flag;
    cin >> flag;

    while (true) {
        if (flag == "write") {
            WaitForSingleObject(hMutex, INFINITE);
            file.open(file_name, std::ios::out | std::ios::app);

            string msg;
            cout << "Type in message: ";
            cin >> msg;

            char message[21];
            for (int i = 0; i < msg.length(); i++) {
                message[i] = msg[i];
            }

            for (int i = msg.length(); i < 21; i++) {
                message[i] = '\0';
            }

            message[20] = '\n';
            ReleaseMutex(hMutex);
            ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);

            if (ReleaseSemaphore(hInputReadySemaphore, 1, NULL) != 1) {
                cout << "you have exceeded the number of messages!";
                ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);
                WaitForSingleObject(hOutputReadySemaphore, INFINITE);
                ReleaseSemaphore(hOutputReadySemaphore, 1, NULL);
                ReleaseSemaphore(hInputReadySemaphore, 1, NULL);

                for (int i = 0; i < 21; i++) {
                    file << message[i];
                }
            }
            else {
                for (int i = 0; i < 21; i++) {
                    file << message[i];
                }
            }

            file.close();
            cout << "\nInput 'write' to write message;" << endl;
            cout<<"Input 'exit' to exit process"<<endl;
            cin >> flag;
        }
        else if (flag == "exit") {
            cout << "Process finished"<<endl;
            break;
        }

        else {
            cout<<"\nIncorrect value!"<<endl;
            cout<<"Input 'write' to write message"<<endl;
            cout<<"Input 'exit' to exit process"<<endl;
            cin >> flag;
        }
    }
}