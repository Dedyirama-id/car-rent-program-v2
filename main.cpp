#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstring>

using namespace std;

#define CARS_DATA_FILE "cars-data.bin"
#define RENT_DATA_FILE "rent-data.bin"

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

void removeCarList(fstream &data)
{
    system("cls");
    fstream temp;
    temp.open("temp.bin", ios::out | ios::binary);
    if (temp.is_open())
    {
        int size = getDataSize(data);
        cout << "[Remove Car]" << endl;
        char regNumber[MAX_STRING_LENGTH];
        cout << "Registration Number: ";
        cin >> regNumber;
        toUpper(regNumber);
        cin.ignore();
        bool found = false;
        for (int i = 1; i <= size; i++)
        {
            CarRentData car = readData(data, i);
            if (strcmp(car.regNumber, regNumber) == 0)
                found = true;
            else
                temp.write(reinterpret_cast<char *>(&car), sizeof(CarRentData));
        }

        if (found)
        {
            data.close();
            remove(CARS_DATA_FILE);
            temp.close();
            rename("temp.bin", CARS_DATA_FILE);
            cout << "Car " << regNumber << " has been removed!" << endl;
            cin.get();
            data.open(CARS_DATA_FILE, ios::out | ios::in | ios::binary);
        }
        else
        {
            cout << "Car not found!" << endl;
            temp.close();
            remove("temp.bin");
            cin.get();
        }
    }
    else
    {
        cout << "Error when creating temporary file" << endl;
        cout << "Please try again!" << endl;
        cin.get();
    }
}

int findPos(fstream &data, char regNumber[])
{
    int size = getDataSize(data);
    for (int i = 1; i <= size; i++)
    {
        CarRentData car = readData(data, i);
        if (strcmp(car.regNumber, regNumber) == 0)
        {
            return i;
        }
    }
    return -1;
}

void rentCar(fstream &data)
{
    system("cls");
    int size = getDataSize(data);
    CarRentData rentCar;

    cout << "[Rent Car]" << endl;
    char regNumber[MAX_STRING_LENGTH];
    cout << "Registration Number: ";
    cin >> regNumber;
    toUpper(regNumber);

    int pos = findPos(data, regNumber);
    cin.ignore();
    if (pos >= 0)
    {
        rentCar = readData(data, pos);
        if (strcmp(rentCar.status, "Not Rented") == 0)
        {
            cout << "Customer Id: ";
            cin >> rentCar.customerId;
            cout << "Customer Name: ";
            cin >> rentCar.customerName;
            cout << "Rent Duration (days): ";
            cin >> rentCar.rentDuration;

            strncpy(rentCar.status, "Rented", MAX_STRING_LENGTH);
            writeData(data, pos, rentCar);
            cout << "Car " << regNumber << " successfully rented!" << endl;
            cin.ignore();
            cin.get();
        }
        else if (rentCar.status == "Rented")
        {
            cout << "Car " << regNumber << " is already rented!" << endl;
            cin.get();
        }
    }
    else
    {
        cout << "Car not found!" << endl;
        cin.get();
        return;
    }
}

void returnCar(fstream &data)
{
    system("cls");
    int size = getDataSize(data);

    cout << "[Return Car]" << endl;
    char regNumber[MAX_STRING_LENGTH];
    cout << "Registration Number: ";
    cin >> regNumber;
    toUpper(regNumber);

    int pos = findPos(data, regNumber);
    CarRentData returnCar;

    cin.ignore();
    if (pos >= 0)
    {
        returnCar = readData(data, pos);
        if (returnCar.status == "Not Rented")
        {
            cout << "Car " << regNumber << " is not rented!" << endl;
            cin.get();
            return;
        }
    }
    else
    {
        cout << "Car not found!" << endl;
        cin.get();
        return;
    }

    int customerId;
    cout << "Customer Id: ";
    cin >> customerId;

    cin.ignore();
    if (returnCar.customerId == customerId)
    {
        strncpy(returnCar.status, "Not Rented", MAX_STRING_LENGTH);
        writeData(data, pos, returnCar);
        cout << "Car " << regNumber << " has been returned!" << endl;

        cin.get();
    }
    else
    {
        cout << "Invalid customer id!" << endl;
        cin.get();
        return;
    }
}

void editCarList(fstream &data)
{
    system("cls");
    int size = getDataSize(data);

    cout << "[Edit Car]" << endl;
    cout << "Registration Number: ";
    char regNumber[MAX_STRING_LENGTH];
    cin >> regNumber;
    toUpper(regNumber);

    int pos = findPos(data, regNumber);
    if (pos >= 0)
    {
        system("cls");

        CarRentData showCar = readData(data, pos);
        cout << "[Car Data]" << endl;
        cout << "- Registration Number: " << showCar.regNumber << endl;
        cout << "- Brand: " << showCar.brand << endl;
        cout << "- Model: " << showCar.model << endl;
        cout << "- Year: " << showCar.year << endl;
        cout << "- Rent Fee (/day): " << showCar.rentFee << endl;
        cout << "- Status: " << showCar.status << endl;

        cin.get();
        CarRentData newCar;
        cout << "\n[New Car Data]" << endl;
        cout << "(leave empty if you don't want to change)" << endl;
        cout << "- Registration Number: ";
        cin.getline(newCar.regNumber, MAX_STRING_LENGTH);
        if (strlen(newCar.regNumber) == 0)
            strcpy(newCar.regNumber, showCar.regNumber);

        cout << "- Brand: ";
        cin.getline(newCar.brand, MAX_STRING_LENGTH);
        if (strlen(newCar.brand) == 0)
            strcpy(newCar.brand, showCar.brand);

        cout << "- Model: ";
        cin.getline(newCar.model, MAX_STRING_LENGTH);
        if (strlen(newCar.model) == 0)
            strcpy(newCar.model, showCar.model);

        char buffer[MAX_STRING_LENGTH];
        cout << "- Year: ";
        cin.getline(buffer, MAX_STRING_LENGTH);
        newCar.year = (strlen(buffer) == 0) ? showCar.year : (int)atoi(buffer);

        cout << "- Rent Fee (/day): ";
        cin.getline(buffer, MAX_STRING_LENGTH);
        newCar.rentFee = (strlen(buffer) == 0) ? showCar.rentFee : (int)atoi(buffer);

        strncpy(newCar.status, showCar.status, MAX_STRING_LENGTH);
        newCar.customerId = showCar.customerId;
        strncpy(newCar.customerName, showCar.customerName, MAX_STRING_LENGTH);
        newCar.rentDuration = showCar.rentDuration;

        writeData(data, pos, newCar);
        cout << "Car " << regNumber << " successfully updated!" << endl;
    }
    else
    {
        cout << "Car not found!" << endl;
    }
    cin.get();
}

int getChoice()
{
    cout << "[Car Rent Services]" << endl;
    cout << "1. Show Car List" << endl;
    cout << "2. Add Car List" << endl;
    cout << "3. Edit car" << endl;
    cout << "4. Remove Car On List" << endl;
    cout << "5. Rent Car" << endl;
    cout << "6. Return Car" << endl;
    cout << "Choose menu (0 to exit): ";
    int input;
    cin >> input;
    cin.ignore();
    return input;
}

void checkDatabase(fstream &data)
{
    data.open(CARS_DATA_FILE, ios::out | ios::in | ios::binary);
    if (data.is_open())
    {
        cout << "database ditemukan" << endl;
    }
    else
    {
        cout << "database tidak ditemukan, membuat database baru!" << endl;
        data.close();
        data.open(CARS_DATA_FILE, ios::trunc | ios::out | ios::in | ios::binary);
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
        case 3:
            editCarList(data);
            break;
        case 4:
            removeCarList(data);
            break;
        case 5:
            rentCar(data);
            break;
        case 6:
            returnCar(data);
            break;
        }
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}