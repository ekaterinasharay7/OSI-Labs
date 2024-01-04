#include <conio.h>
#include <Windows.h>
#include <iostream>
#include "employee.h"


using std::cout;
using std::cin;


int main() {
    HANDLE hStartEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, "Process Started");
    if (hStartEvent == NULL)
    {
        cout << "Start Event Error \n";
        return GetLastError();
    }
    SetEvent(hStartEvent);
    HANDLE hPipe = CreateFileA("\\\\.\\pipe\\pipe_name", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Creation of the named pipe failed.\n The last error code: ";
        return GetLastError();
    }
    while (true) {
        int chosen_option = 0;
        cout << "Change data - 1; \nRead data - 2; \nExit - 3 \n>>>";
        cin >> chosen_option;
        if (chosen_option == 3) {
            break;
        }
        else if (chosen_option == '1') {
            DWORD dwBytesWritten;
            DWORD dwBytesReaden;
            int ID;
            cout << "Please, enter employee ID \n>>>";
            cin >> ID;
            int message_to_send = ID * 10 + chosen_option;
            bool checker = WriteFile(
                hPipe,
                &message_to_send,
                sizeof(message_to_send),
                &dwBytesWritten,
                NULL);
            if (checker) {
                cout << "Message was sent.\n";
            }
            else {
                cout << "Message wasn't sent.\n";
            }
            employee* emp = new employee();
            ReadFile(hPipe, emp, sizeof(employee), &dwBytesReaden, NULL);

            cout << "ID of employee: " << emp->num << ".\nNAME of employee: " << emp->name << ".\nHOURS of employee: " << emp->hours << ".\n";
            cout << "Please,enter NEW Name:\n";
            cin >> emp->name;
            cout << "Please, ENTER NEW Hours:\n";
            cin >> emp->hours;
            checker = WriteFile(hPipe, emp, sizeof(employee), &dwBytesWritten, NULL);
            if (checker) {
                std::cout << "Message was sent.\n";
            }
            else {
                std::cout << "Message wasn't sent.\n";
            }
            std::cout << "Press any key to confirm the option...\n";
            _getch();
            message_to_send = 1;
            WriteFile(hPipe, &message_to_send, sizeof(message_to_send), &dwBytesWritten, NULL);
        }
        else if (chosen_option == 2) {
            DWORD dwBytesWritten;
            DWORD dwBytesReaden;
            int ID;
            std::cout << "Please,enter an ID of employee:\n";
            std::cin >> ID;
            int message_to_send = ID * 10 + chosen_option;
            bool checker = WriteFile(hPipe, &message_to_send, sizeof(message_to_send), &dwBytesWritten, NULL);
            if (checker) {
                std::cout << "Message was sent.\n";
            }
            else {
                std::cout << "Message wasn't sent.\n";
            }
            employee* emp = new employee();
            ReadFile(hPipe, emp, sizeof(employee), &dwBytesReaden, NULL);
            std::cout << "employee ID: " << emp->num << ".\nemployee NAME: " << emp->name << ".\nemployee HOURS: " << emp->hours << ".\n";
            std::cout << "Press any key to confirm the option...\n";
            system("pause");
            message_to_send = 1;
            WriteFile(hPipe, &message_to_send, sizeof(message_to_send), &dwBytesWritten, NULL);
        }
    }
    int a;

    cin >> a;

    return 0;
}