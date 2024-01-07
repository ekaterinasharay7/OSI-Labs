#include <iostream>
#include <fstream>
#include "..//osi-laba1-test1/employee.h"


int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "\nUsage: " << argv[0] << " <binary_file_name> <num_records> \n";
        return 1;
    }

    const char* binaryFileName = argv[1];
    int numRecords = atoi(argv[2]);

    if (numRecords <= 0) {
        std::cerr << "Number of records must be a positive integer. \n";
        return 1;
    }

    // Îòêðûâàåì áèíàðíûé ôàéë äëÿ äàëüíåéøåé ðàáîòû
    std::ofstream binaryFile(binaryFileName, std::ios::binary);

    if (!binaryFile) {
        std::cerr << "\nError: Unable to open the binary file for writing. \n";
        return 1;
    }

    employee emp;
    for (int i = 0; i < numRecords; ++i) {
        std::cout << "\nEmployee #" << i + 1 << " details: \n";
        std::cout << "ID: ";
        std::cin >> emp.num;
        std::cout << "Name (up to 10 characters): ";
        std::cin >> emp.name;
        std::cout << "Hours worked: ";
        std::cin >> emp.hours;

        binaryFile.write(reinterpret_cast<const char*>(&emp), sizeof(emp));
    }

    binaryFile.close();

    std::cout << "\nBinary file '" << binaryFileName << "' created successfully with " << numRecords << " records. \n";

    return 0;
}
