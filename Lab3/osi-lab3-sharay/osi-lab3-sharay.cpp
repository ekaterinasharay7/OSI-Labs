#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
using namespace std;


int* arr;
int arraySize;

HANDLE* markerThreads;
HANDLE mainEvent;
HANDLE* markerEvents;
HANDLE* StopMarkerEvents;
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);

struct MarkerThreadArgs {
    int markerId;//порядковый номер маркера
    bool terminate = FALSE;//подали ли сигнал на завершение работы
};

// Marker thread
DWORD WINAPI MarkerThread(LPVOID lpParam) {
    MarkerThreadArgs* t_args = reinterpret_cast<MarkerThreadArgs*>(lpParam);
    int markerId = t_args->markerId;

    random_device rd;
    mt19937 rng(rd() + markerId);
    uniform_int_distribution<int> dist(0, arraySize - 1);

    int counter = 0;
    while (true) {
        // очищаем отмеченные элементы и закрываем поток
        if (t_args->terminate == TRUE) {
            for (int i = 0; i < arraySize; i++) {
                if (arr[i] == markerId) {//если совпал индекс => закидываем 0
                    arr[i] = 0;
                }
            }
            return 0;
        }
        // Ждем пока main подаст сигнал на начало работы
        WaitForSingleObject(mainEvent, INFINITE);

        int indexToMark = dist(rng);

        if (arr[indexToMark] == 0) {
            // Отмечаем элементы
            WaitForSingleObject(mutex, INFINITE);
            Sleep(5);
            arr[indexToMark] = markerId;
            ReleaseMutex(mutex);
            counter++;
            Sleep(5);
        }
        else {
            // Посылаем уведомление к main что выполнить невозможно
            WaitForSingleObject(mutex, INFINITE);
            cout << "Marker поток номер: " << markerId << "  Количество помеченных элементов: " << counter << "\n";
            ReleaseMutex(mutex);
            ResetEvent(StopMarkerEvents[markerId - 1]);
            SetEvent(markerEvents[markerId - 1]);
            WaitForSingleObject(StopMarkerEvents[markerId - 1], INFINITE);
        }
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Здравствуйте! Введите размерность массива: ";
    cin >> arraySize;

    arr = new int[arraySize];
    memset(arr, 0, sizeof(int) * arraySize);

    cout << "Введите количество потоков marker: ";
    int numMarkerThreads;
    cin >> numMarkerThreads;

    markerThreads = new HANDLE[numMarkerThreads];
    markerEvents = new HANDLE[numMarkerThreads];
    StopMarkerEvents = new HANDLE[numMarkerThreads];
    MarkerThreadArgs* thread_args = new MarkerThreadArgs[numMarkerThreads];

    // создаем главное событие
    mainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // создаем наш поток
    for (int i = 0; i < numMarkerThreads; i++) {
        thread_args[i].markerId = i + 1;
        markerEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        StopMarkerEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        markerThreads[i] = CreateThread(NULL, 0, MarkerThread, &thread_args[i], 0, NULL);
    }

    // Уведомляем о старте
    SetEvent(mainEvent);

    int TerminatedThedsCount = 0;
    int markerId;
    bool thread_to_terminate_choice = TRUE;
    DWORD waitResult;

    for (int mark_iter = 0; mark_iter < numMarkerThreads; mark_iter++) {
        SetEvent(mainEvent);

        // Ждем пока все потоки уведомят о невозмодности или заверешении
        waitResult = WaitForMultipleObjects(numMarkerThreads, markerEvents, TRUE, INFINITE);

        

        if (waitResult == WAIT_OBJECT_0) {
            ResetEvent(mainEvent);

            // Выводим содержимое массива
            cout << "\nСодержимое массива : ";
            for (int i = 0; i < arraySize; i++) {
                cout << arr[i] << " ";
            }

            while (thread_to_terminate_choice) {
                // Запрашиваем номер marker потока
                cout << "\n\nВведите номер потока, работу которого хотите завершить (1-" << numMarkerThreads << "): ";
                cin >> markerId;

                if (thread_args[markerId - 1].terminate == TRUE) {
                    cout << "\nПоток под номером: " << markerId << " Уже завершен ранее.Выберите другой номер потока для завершения.\n";
                    continue;
                }
                else {
                    thread_args[markerId - 1].terminate = TRUE;

                    for (int i = 0; i < numMarkerThreads; i++) {
                        SetEvent(StopMarkerEvents[i]);
                        if (thread_args[i].terminate == TRUE) {
                            SetEvent(markerEvents[i]);
                        }
                    }
                    break;
                }
            }
            WaitForSingleObject(markerThreads[markerId - 1], INFINITE); 
        }
    }

    cout << "Все потоки завершены \n";

    // Все очищаем
    for (int i = 0; i < numMarkerThreads; i++) {
        CloseHandle(markerThreads[i]);
        CloseHandle(markerEvents[i]);
        CloseHandle(StopMarkerEvents[i]);
    }
    CloseHandle(mutex);

    delete[] arr;
    delete[] markerThreads;
    delete[] markerEvents;
    delete[] StopMarkerEvents;

    return 0;
}