#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstring>

using namespace std;

#define dataFileName "cars-data.bin"

const int MAX_STRING_LENGTH = 50;

struct CarRentData
{
    char regNumber[MAX_STRING_LENGTH];
    char brand[MAX_STRING_LENGTH];
    char model[MAX_STRING_LENGTH];
    int year;
    int rentFee;
    char status[MAX_STRING_LENGTH];
    int customerId;
    char customerName[MAX_STRING_LENGTH];
    int rentDuration;
};

void toUpper(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        str[i] = toupper(str[i]);
    }
}

void capitalize(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (i == 0 || str[i - 1] == ' ')
        {
            str[i] = std::toupper(str[i]);
        }
        else
        {
            str[i] = std::tolower(str[i]);
        }
    }
}

CarRentData readData(fstream &data, int posisi)
{
    CarRentData readCar;
    data.seekg((posisi - 1) * sizeof(CarRentData), ios::beg);
    data.read(reinterpret_cast<char *>(&readCar), sizeof(CarRentData));
    return readCar;
}

void writeData(fstream &data, int posisi, CarRentData &inputCar)
{
    data.seekp((posisi - 1) * sizeof(CarRentData), ios::beg);
    data.write(reinterpret_cast<char *>(&inputCar), sizeof(CarRentData));
}

int getDataSize(fstream &data)
{
    int start, end;
    data.seekg(0, ios::beg);
    start = data.tellg();
    data.seekg(0, ios::end);
    end = data.tellg();
    return (end - start) / sizeof(CarRentData);
}

void showCarList(fstream &data)
{
    system("cls");

    int size = getDataSize(data);
    CarRentData showCar;

    cout << "[Car List]" << endl;
    for (int i = 1; i <= size; i++)
    {
        showCar = readData(data, i);
        cout << i << ". Registration Number: " << showCar.regNumber << endl;
        cout << "   - Brand \t: " << showCar.brand << endl;
        cout << "   - Model \t: " << showCar.model << endl;
        cout << "   - Year \t: " << showCar.year << endl;
        cout << "   - Rent Fee \t: " << showCar.rentFee << endl;
        cout << "   - Status \t: " << showCar.status << endl;
    }
    cin.get();
}

void addCarList(fstream &data)
{
    int size = getDataSize(data);
    system("cls");
    cout << "[Add New Car]" << endl;
    CarRentData newCar;

    cout << "Registration Number: ";
    cin >> newCar.regNumber;
    toUpper(newCar.regNumber);
    cin.ignore();
    cout << "Brand: ";
    cin.getline(newCar.brand, MAX_STRING_LENGTH);
    capitalize(newCar.brand);
    cout << "Model: ";
    cin.getline(newCar.model, MAX_STRING_LENGTH);
    capitalize(newCar.model);
    cout << "year: ";
    cin >> newCar.year;
    cout << "Rent Fee: ";
    cin >> newCar.rentFee;
    strncpy(newCar.status, "Not Rented", MAX_STRING_LENGTH);
    newCar.customerId = 0;
    strncpy(newCar.customerName, "-", MAX_STRING_LENGTH);
    newCar.rentDuration = 0;

    writeData(data, size + 1, newCar);
    cin.ignore();
    cin.get();
}

int getChoice()
{
    cout << "[Car Rent Services]" << endl;
    cout << "1. Show Car List" << endl;
    cout << "2. Add Car List" << endl;
    cout << "3. Remove Car On List" << endl;
    cout << "4. Rent Car" << endl;
    cout << "5. Return Car" << endl;
    cout << "Choose menu (0 to exit): ";
    int input;
    cin >> input;
    cin.ignore();
    return input;
}

void checkDatabase(fstream &data)
{
    data.open(dataFileName, ios::out | ios::in | ios::binary);
    if (data.is_open())
    {
        cout << "database ditemukan" << endl;
    }
    else
    {
        cout << "database tidak ditemukan, membuat database baru!" << endl;
        data.close();
        data.open(dataFileName, ios::trunc | ios::out | ios::in | ios::binary);
    }
}

int main()
{
    fstream data;
    checkDatabase(data);

    while (true)
    {
        system("cls");
        int choice = getChoice();
        switch (choice)
        {
        case 0:
            cout << "Exit" << endl;
            return 0;
        case 1:
            showCarList(data);
            break;
        case 2:
            addCarList(data);
            break;
        }
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}