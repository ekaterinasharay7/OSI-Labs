#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <algorithm>
#include "employee.h"


HANDLE* hStarted;
HANDLE* hPipe;
HANDLE* hThreads;
HANDLE* hSemaphore;
employee* emps;
int number_of_employees;
int number_of_clients;
std::string file_name;

DWORD WINAPI operations(LPVOID pipe) {
    HANDLE hPipe = (HANDLE)pipe;
    DWORD dwBytesRead;
    DWORD dwBytesWrite;

    int message;
    while (true) {
        if (!ReadFile(
            hPipe,
            &message,
            sizeof(message),
            &dwBytesRead,
            NULL))
        {
        }
        else {
            int ID = message / 10;
            int chosen_option = message % 10;
            if (chosen_option == 1) {
                for (int i = 0; i < number_of_clients; i++)
                {
                    WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
                }
                employee* emp_to_push = new employee();
                emp_to_push->num = emps[ID - 1].num;
                emp_to_push->hours = emps[ID - 1].hours;
                strcpy_s(emp_to_push->name, emps[ID - 1].name);
                bool checker = WriteFile(hPipe, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
                if (checker) {
                    std::cout << "Data to modify was sent.\n";
                }
                else {
                    std::cout << "Data to modify wasn't sent.\n";
                }
                ReadFile(hPipe, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
                emps[ID - 1].hours = emp_to_push->hours;
                strcpy_s(emps[ID - 1].name, emp_to_push->name);
                std::ofstream file;
                file.open(file_name);
                for (int i = 0; i < number_of_employees; i++)
                    file << emps[i].num << " " << emps[i].name << " " << emps[i].hours << "\n";
                file.close();
                int msg;
                ReadFile(hPipe, &msg, sizeof(msg), &dwBytesWrite, NULL);
                if (msg == 1) {
                    for (int i = 0; i < number_of_clients; i++)
                    {
                        ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
                    }
                }
            }
            if (chosen_option == 2) {

                WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
                employee* emp_to_push = new employee();
                emp_to_push->num = emps[ID - 1].num;
                emp_to_push->hours = emps[ID - 1].hours;
                strcpy_s(emp_to_push->name, emps[ID - 1].name);
                bool checker = WriteFile(hPipe, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
                if (checker) {
                    std::cout << "Data to read was sent.\n";
                }
                else {
                    std::cout << "Data to read wasn't sent.\n";
                }
                int msg;
                ReadFile(hPipe, &msg, sizeof(msg), &dwBytesWrite, NULL);
                if (msg == 1)
                    ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
            }
        }
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

}

int main() {
    std::cout << "Please, enter  file name:\n";
    std::cin >> file_name;
    file_name = "emps.txt";
    std::cout << "Please, enter number of employees:\n";
    std::cin >> number_of_employees;
    std::ofstream file;
    emps = new employee[number_of_employees];
    for (int i = 0; i < number_of_employees; i++)
    {
        std::cout << "Input " << i + 1 << " employee ID:\n";
        std::cin >> emps[i].num;
        std::cout << "Please, enter employee NAME:\n";
        std::string tmp;
        std::cin >> emps[i].name;
        std::cout << "Please,enter employee HOURS:\n";
        std::cin >> emps[i].hours;
    }
    file.open(file_name);
    for (int i = 0; i < number_of_employees; i++)
        file << emps[i].num << " " << emps[i].name << " " << emps[i].hours << "\n";
    file.close();
    std::ifstream file_input;
    file_input.open(file_name);
    for (int i = 0; i < number_of_employees; i++)
    {
        int id;
        char name[10];
        double hours;
        file_input >> id >> name >> hours;
        std::cout << "\nEmployee ID: " << id << ".\nEmployee NAME: " << name << ".\nEmployee HOURS: " << hours << ".\n";

    }
    file_input.close();


    std::cout << "Please, enter NUMBER of clients:\n";
    std::cin >> number_of_clients;
    hStarted = new HANDLE[number_of_clients];
    hSemaphore = new HANDLE[number_of_employees];
    for (int i = 0; i < number_of_employees; i++)
    {
        hSemaphore[i] = CreateSemaphoreA(NULL, number_of_clients, number_of_clients, "hSemahpore");
    }
    for (int i = 0; i < number_of_clients; ++i)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
       
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, LPSTR("Client.exe"), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
        hStarted[i] = CreateEventA(NULL, FALSE, FALSE, "Process Started");
        CloseHandle(pi.hProcess);
    }
    WaitForMultipleObjects(number_of_clients, hStarted, TRUE, INFINITE);
    hPipe = new HANDLE[number_of_clients];
    hThreads = new HANDLE[number_of_clients];
    for (int i = 0; i < number_of_clients; i++)
    {
        hPipe[i] = CreateNamedPipeA("\\\\.\\pipe\\pipe_name", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            std::cout << "Creation of the named pipe failed.\n The last error code: " << GetLastError() << "\n";
            std::cout << "Press any char to finish server: ";
            _getch();
            return (0);
        }
        if (!ConnectNamedPipe(hPipe[i], (LPOVERLAPPED)NULL))
        {
            std::cout << "The connection failed.\nThe last error code: " << GetLastError() << "\n";
            CloseHandle(hPipe[i]);
            std::cout << "Press any char to finish the server: ";
            _getch();
            return (0);
        }
        hThreads[i] = CreateThread(NULL, 0, operations, static_cast<LPVOID>(hPipe[i]), 0, NULL);

    }
    WaitForMultipleObjects(number_of_clients, hThreads, TRUE, INFINITE);
    std::cout << "All clients has ended their work.";
    file_input.open(file_name);
    for (int i = 0; i < number_of_employees; i++)
    {
        int id;
        char name[10];
        double hours;
        file_input >> id >> name >> hours;
        std::cout << "\nemployee ID: " << id << ".\nemployee NAME: " << name << ".\nemployee HOURS: " << hours << ".\n";

    }
    file_input.close();
    std::cout << "Press any key to exit...\n";
    _getch();
    return 0;
}