//sharay ekaterina - 7 group
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>


    using std::cout;

    CRITICAL_SECTION cs;
    int* numbers;
    int size;

    struct ThreadArgs {
        int max;
        int min;
        int average = 0;
    };

    DWORD WINAPI MinMaxThread(LPVOID lpParam) {
        // cout << "min_max thread is started.\n";

        ThreadArgs* args = reinterpret_cast<ThreadArgs*>(lpParam);

        // Find the minimum and maximum elements && Save their positions
        args->min = numbers[0];
        args->max = numbers[0];

        for (int i = 1; i < size; ++i) {
            if (numbers[i] < args->min) {
                args->min = numbers[i];
            }
            Sleep(7);

            if (numbers[i] > args->max) {
                args->max = numbers[i];
            }
            Sleep(7);
        }
        EnterCriticalSection(&cs);
        cout << "Minimum value: " << args->min << std::endl;
        cout << "Maximum value: " << args->max << std::endl;
        LeaveCriticalSection(&cs);

        // cout << "min_max thread is finished.\n";
        return 0;
    }

    DWORD WINAPI AverageThread(LPVOID lpParam) {
        // cout << "average thread is started.\n";

        ThreadArgs* args = reinterpret_cast<ThreadArgs*>(lpParam);

        // Calculate the sum of elements and the average value
        for (int i = 0; i < size; ++i) {
            args->average += numbers[i];
            Sleep(12);
        }
        args->average /= size;
        EnterCriticalSection(&cs);
        cout << "Average value: " << args->average << std::endl;
        LeaveCriticalSection(&cs);
        // cout << "Average thread is finished.\n";
        return 0;
    }

    int main() {
        cout << "\nEnter the size of the array: ";
        std::cin >> size;

        numbers = new int[size];
        InitializeCriticalSection(&cs);
        cout << "Enter " << size << " integers:\n>>> ";
        for (int i = 0; i < size; ++i) {
            std::cin >> numbers[i];
        }

        // Create min_max and average threads
        ThreadArgs thread_args;
        HANDLE hMinMaxThread = CreateThread(NULL, 0, MinMaxThread, &thread_args, 0, NULL);
        HANDLE hAverageThread = CreateThread(NULL, 0, AverageThread, &thread_args, 0, NULL);

        if (hMinMaxThread == NULL || hAverageThread == NULL) {
            cout << "Error creating threads.\n";
            return GetLastError();
        }

        // Wait for both threads to finish
        WaitForSingleObject(hMinMaxThread, INFINITE);
        WaitForSingleObject(hAverageThread, INFINITE);

        DeleteCriticalSection(&cs);
        CloseHandle(hMinMaxThread);
        CloseHandle(hAverageThread);

        // Replace min and max elements with the average
        for (int i = 0; i < size; i++) {
            if ((numbers[i] == thread_args.max) || (numbers[i] == thread_args.min)) {
                numbers[i] = thread_args.average;
            }
        }

        cout << "Result: ";
        for (int i = 0; i < size; i++) {
            cout << numbers[i] << ";  ";
        }

        cout << "\n\n";

        delete[] numbers;

        return 0;
    }


