#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <windows.h>
#include <fstream>
#include "employee.h"


int main() {
    std::string binaryFileName;
    int numRecords;

    std::cout << "\n\nEnter the name of the binary file: ";
    std::cin >> binaryFileName;

    std::cout << "Enter the number of records to create: ";
    std::cin >> numRecords;


    // создаем процесс Creator  и ждем пока отработает
    std::string commandLine = "Creator.exe " + binaryFileName + " " + std::to_string(numRecords);

    STARTUPINFO si;
    PROCESS_INFORMATION piCreator;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piCreator))
    {
        std::cerr << "Error: Failed to execute Creator utility." << std::endl;
        return 1;
    }

    WaitForSingleObject(piCreator.hProcess, INFINITE);
    CloseHandle(piCreator.hThread);
    CloseHandle(piCreator.hProcess);

    std::cout << "\nContents of the binary file: \n------------------------\n";

    std::ifstream binaryFile(binaryFileName, std::ios::binary);
    if (!binaryFile) {
        std::cerr << "Error: Unable to open the input binary file for reading." << std::endl;
        return 1;
    }

    //собрать инфу про рабочих из бинарного  файла
    employee emp;
    while (binaryFile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        
        std::cout << "Employee ID: " << emp.num << "\n";
        std::cout << "Employee Name: " << emp.name << "\n";
        std::cout << "Hours Worked: " << emp.hours << "\n";
        std::cout << "------------------------\n";
    }
    binaryFile.close();

    std::string reportFileName;
    int hour_oplata;

    // Забиарем у пользователя название отчета и почасовую оплату
    std::cout << "\nEnter the name of the report file: ";
    std::cin >> reportFileName;
    std::cout << "\nEnter the hourly wage: ";
    std::cin >> hour_oplata;

    // Меняем командную строку для создания нового процесса
    commandLine = "Reporter.exe " + binaryFileName + " " + reportFileName + " " + std::to_string(hour_oplata);

    // Создаем процесс Reporter
    PROCESS_INFORMATION piReporter;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&piReporter, sizeof(piReporter));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piReporter)) {
        std::cout << "\nError: Failed to execute Reporter utility. \n";
        return 1;
    }

    WaitForSingleObject(piReporter.hProcess, INFINITE);
    CloseHandle(piReporter.hThread);
    CloseHandle(piReporter.hProcess);

    std::cout << "\n\n";

    return 0;
}