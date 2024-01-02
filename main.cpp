#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstring>
#include <conio.h>
#include <ctime>

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
    time_t rentDate;
};

CarRentData readData(fstream &data, int posisi);
void toUpper(char *str);
void capitalize(char *str);
void writeData(fstream &data, int posisi, CarRentData &inputCar);
int getDataSize(fstream &data);
void showCarList(fstream &data);
void addCarList(fstream &data);
void removeCarList(fstream &data);
int findPos(fstream &data, char regNumber[]);
void rentCar(fstream &data);
void returnCar(fstream &data);
void editCarList(fstream &data);
void makeInvoice(fstream &data, CarRentData &car, string mode, time_t returnDate = 0);
int getChoice();
void checkDatabase(fstream &data);

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
            str[i] = std::toupper(str[i]);
        else
            str[i] = std::tolower(str[i]);
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
        cout << i << ". Reg Number\t: " << showCar.regNumber << endl;
        cout << "   - Brand \t: " << showCar.brand << endl;
        cout << "   - Model \t: " << showCar.model << endl;
        cout << "   - Year \t: " << showCar.year << endl;
        cout << "   - Rent Fee \t: Rp" << showCar.rentFee << endl;
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
    cout << "Rent Fee: Rp";
    cin >> newCar.rentFee;
    strncpy(newCar.status, "Not Rented", MAX_STRING_LENGTH);
    newCar.customerId = 0;
    strncpy(newCar.customerName, "-", MAX_STRING_LENGTH);
    newCar.rentDuration = 0;
    newCar.rentDate = 0;

    writeData(data, size + 1, newCar);
    cout << "Car added successfully!" << endl;
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

void makeInvoice(fstream &data, CarRentData &car, string mode, time_t returnDate)
{
    system("cls");
    if (mode == "rent")
    {
        cout << "================================================" << endl;
        cout << "                CAR RENT INVOICE                " << endl;
        cout << "================================================" << endl;
        cout << "Customer\t: " << car.customerName << endl;
        cout << "Customer ID\t: " << car.customerId << endl;
        cout << "Reg. Number\t: " << car.regNumber << endl;
        cout << "Brand\t\t: " << car.brand << endl;
        cout << "Model\t\t: " << car.model << endl;
        cout << "Rent Fee\t: Rp" << car.rentFee << " /day" << endl;
        cout << "Rent Duration\t: " << car.rentDuration << " days" << endl;
        cout << "================================================" << endl;
    }
    else if (mode == "return")
    {
        tm *rentDate = localtime(&car.rentDate);
        char sRentDate[80];
        strftime(sRentDate, sizeof(sRentDate), "%Y-%m-%d %H:%M:%S", rentDate);

        char sReturnDate[80];
        strftime(sReturnDate, sizeof(sReturnDate), "%Y-%m-%d %H:%M:%S", localtime(&returnDate));

        cout << "================================================" << endl;
        cout << "                CAR RETURN INVOICE               " << endl;
        cout << "================================================" << endl;
        cout << "Customer\t: " << car.customerName << endl;
        cout << "Customer ID\t: " << car.customerId << endl;
        cout << "Reg. Number\t: " << car.regNumber << endl;
        cout << "Brand\t\t: " << car.brand << endl;
        cout << "Model\t\t: " << car.model << endl;
        cout << "Rent Date\t: " << sRentDate << endl;
        cout << "Return Date\t: " << sReturnDate << endl;
        cout << "================================================" << endl;
    }
}

void rentCar(fstream &data)
{
    system("cls");
    int size = getDataSize(data);
    CarRentData rentCar;

    cout << "[Rent Car]" << endl;
    char regNumber[MAX_STRING_LENGTH];
    cout << "Registration Number\t: ";
    cin >> regNumber;
    toUpper(regNumber);

    int pos = findPos(data, regNumber);
    cin.ignore();
    if (pos >= 0)
    {
        rentCar = readData(data, pos);
        if (strcmp(rentCar.status, "Not Rented") == 0)
        {
            cout << "Customer Id \t\t: ";
            cin >> rentCar.customerId;
            cout << "Customer Name \t\t: ";
            cin >> rentCar.customerName;
            capitalize(rentCar.customerName);
            cout << "Rent Duration (days) \t: ";
            cin >> rentCar.rentDuration;

            int fee = rentCar.rentDuration * rentCar.rentFee;
            int pay;
            while (true)
            {
                makeInvoice(data, rentCar, "rent");
                cout << "Total Fee \t: Rp" << fee << endl;
                cout << "Payment \t: Rp";
                cin >> pay;
                if (pay >= fee)
                {
                    break;
                }
                else
                {
                    cout << "The amount paid is less than the total fee!" << endl;
                    cout << "(ESC to back to main menu)" << endl;
                    cin.ignore();
                    if (_getch() == 27)
                        return;
                }
            }

            int refund = pay - fee;
            cout << "Refund \t\t: Rp" << refund << endl;

            strncpy(rentCar.status, "Rented", MAX_STRING_LENGTH);
            rentCar.rentDate = time(0);
            writeData(data, pos, rentCar);
            cout << "================== THANK YOU! ==================" << endl;

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
        char confirm;
        cout << "Are you sure you want to return this car? (y/n) ";
        cin >> confirm;
        cin.ignore(static_cast<unsigned int>(numeric_limits<streamsize>::max()), '\n');
        while (true)
        {
            if (confirm == 'Y' || confirm == 'y')
            {
                strncpy(returnCar.status, "Not Rented", MAX_STRING_LENGTH);
                writeData(data, pos, returnCar);
                time_t now = time(0);
                makeInvoice(data, returnCar, "return", now);
                cout << "Car " << regNumber << " has been returned!" << endl;
                cin.get();
                return;
            }
            else if (confirm == 'N' || confirm == 'n')
            {
                cout << "Return cancelled!" << endl;
                cin.get();
                return;
            }
            else
            {
                cout << "Invalid input!" << endl;
                cout << "(ESC to back to main menu)" << endl;
                cin.ignore();
                if (_getch() == 27)
                    return;
            }
        }
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
        CarRentData showCar = readData(data, pos);
        if (showCar.status == "Not Rented")
        {
            system("cls");

            cout << "[Car Data]" << endl;
            cout << "- Registration Number: " << showCar.regNumber << endl;
            cout << "- Brand: " << showCar.brand << endl;
            cout << "- Model: " << showCar.model << endl;
            cout << "- Year: " << showCar.year << endl;
            cout << "- Rent Fee (/day): Rp" << showCar.rentFee << endl;
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
            newCar.rentDate = showCar.rentDate;

            writeData(data, pos, newCar);
            cout << "Car " << regNumber << " successfully updated!" << endl; 
        } else {
            cout << "Cannot change the data of active rented car!" << endl;
        }
    }
    else
    {
        cout << "Car not found!" << endl;
    }
    cin.ignore();
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
    system("cls");
    data.open(CARS_DATA_FILE, ios::out | ios::in | ios::binary);
    if (data.is_open())
    {
        cout << "Database found!" << endl;
    }
    else
    {
        cout << "Database not found! Creating new one..." << endl;
        data.close();
        data.open(CARS_DATA_FILE, ios::trunc | ios::out | ios::in | ios::binary);
    }
    cout << "[enter] to continue! ";
    cin.get();
}
