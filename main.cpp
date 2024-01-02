#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstring>
#include <conio.h>
#include <ctime>
#include "ansi_escape_sequences.h"

using namespace std;

#define CARS_DATA_FILE "cars-data.bin"
#define RENT_DATA_FILE "rent-data.bin"
#define HISTORY_FILE "history.bin"

const int MAX_STRING_LENGTH = 50;
const float penaltyPercentage = 1.25f;
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

CarRentData readData(std::fstream &data, int posisi);
void showCarList(std::fstream &data);
void addCarList(std::fstream &data);
void editCarList(std::fstream &data);
void removeCarList(std::fstream &data);
void rentCar(std::fstream &data);
void returnCar(std::fstream &data);
int getDataSize(std::fstream &data);
void writeData(std::fstream &data, int posisi, CarRentData &inputCar);
int getChoice();
void checkDatabase(std::fstream &data);
void updateHistory(CarRentData &car, char time[], std::string action);
void makeInvoice(std::fstream &data, CarRentData &car, std::string mode, time_t returnDate = 0);
int findPos(std::fstream &data, char regNumber[]);
void toUpper(char *str);
void capitalize(char *str);

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
            cout << GRAY << ITALIC << "Exit" << RESET << endl;
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

CarRentData readData(fstream &data, int posisi)
{
    CarRentData readCar;
    data.seekg((posisi - 1) * sizeof(CarRentData), ios::beg);
    data.read(reinterpret_cast<char *>(&readCar), sizeof(CarRentData));
    return readCar;
}

void showCarList(fstream &data)
{
    system("cls");

    int size = getDataSize(data);
    CarRentData showCar;

    cout << BG_WHITE << BLACK << "=================== CAR LIST ===================" << RESET << endl;
    for (int i = 1; i <= size; i++)
    {
        showCar = readData(data, i);
        cout << i << ". Reg Number\t: " << showCar.regNumber << endl;
        cout << "   - Brand \t: " << showCar.brand << endl;
        cout << "   - Model \t: " << showCar.model << endl;
        cout << "   - Year \t: " << showCar.year << endl;
        cout << "   - Rent Fee \t: Rp" << showCar.rentFee << endl;
        cout << "   - Status \t: " << showCar.status << endl;
        cout << "================================================" << endl;
    }
    cout << GRAY << ITALIC << "[enter] to continue... ";
    cin.get();
}

void writeData(fstream &data, int posisi, CarRentData &inputCar)
{
    data.seekp((posisi - 1) * sizeof(CarRentData), ios::beg);
    data.write(reinterpret_cast<char *>(&inputCar), sizeof(CarRentData));
}

void addCarList(fstream &data)
{
    system("cls");
    cout << BG_WHITE << BLACK << "================= ADD CAR LIST =================" << RESET << endl;

    int size = getDataSize(data);
    CarRentData newCar;

    cout << "Reg. Number\t: ";
    cin >> newCar.regNumber;
    toUpper(newCar.regNumber);
    cin.ignore();
    cout << "Brand\t\t: ";
    cin.getline(newCar.brand, MAX_STRING_LENGTH);
    capitalize(newCar.brand);
    cout << "Model\t\t: ";
    cin.getline(newCar.model, MAX_STRING_LENGTH);
    capitalize(newCar.model);
    cout << "year\t\t: ";
    cin >> newCar.year;
    cout << "Rent Fee\t: Rp";
    cin >> newCar.rentFee;
    strncpy(newCar.status, "Not Rented", MAX_STRING_LENGTH);
    newCar.customerId = 0;
    strncpy(newCar.customerName, "-", MAX_STRING_LENGTH);
    newCar.rentDuration = 0;
    newCar.rentDate = 0;

    writeData(data, size + 1, newCar);
    cout << "================================================" << endl;
    cout << GREEN << ITALIC << "Car added successfully!" << RESET << endl;
    cout << GRAY << ITALIC << "[enter] to continue... " << RESET;
    cin.ignore();
    cin.get();
}

void editCarList(fstream &data)
{
    system("cls");
    int size = getDataSize(data);

    cout << BG_WHITE << BLACK << "=================== EDIT CAR ===================" << RESET << endl;
    cout << "Registration Number: ";
    char regNumber[MAX_STRING_LENGTH];
    cin >> regNumber;
    toUpper(regNumber);

    int pos = findPos(data, regNumber);
    if (pos >= 0)
    {
        CarRentData showCar = readData(data, pos);
        if (strcmp(showCar.status, "Not Rented") == 0)
        {
            system("cls");

            cout << BG_BLUE << WHITE "=================== CAR DATA ===================" << RESET << endl;
            cout << "- Reg. Number\t\t: " << showCar.regNumber << endl;
            cout << "- Brand\t\t\t: " << showCar.brand << endl;
            cout << "- Model\t\t\t: " << showCar.model << endl;
            cout << "- Year\t\t\t: " << showCar.year << endl;
            cout << "- Rent Fee (/day)\t: Rp" << showCar.rentFee << endl;
            cout << "- Status\t\t: " << showCar.status << endl;

            cin.get();
            CarRentData newCar;
            cout << BG_GREEN << WHITE << "\n=================== NEW CAR DATA ===================" << RESET << endl;
            cout << GRAY << ITALIC << "[Leave empty] if you don't want to change!" << RESET << endl;
            cout << "- Reg. Number\t\t: ";
            cin.getline(newCar.regNumber, MAX_STRING_LENGTH);
            toUpper(newCar.regNumber);
            if (strlen(newCar.regNumber) == 0)
                strcpy(newCar.regNumber, showCar.regNumber);

            cout << "- Brand\t\t\t: ";
            cin.getline(newCar.brand, MAX_STRING_LENGTH);
            capitalize(newCar.brand);
            if (strlen(newCar.brand) == 0)
                strcpy(newCar.brand, showCar.brand);

            cout << "- Model\t\t\t: ";
            cin.getline(newCar.model, MAX_STRING_LENGTH);
            capitalize(newCar.model);
            if (strlen(newCar.model) == 0)
                strcpy(newCar.model, showCar.model);

            char buffer[MAX_STRING_LENGTH];
            cout << "- Year\t\t\t: ";
            cin.getline(buffer, MAX_STRING_LENGTH);
            newCar.year = (strlen(buffer) == 0) ? showCar.year : (int)atoi(buffer);

            cout << "- Rent Fee (/day)\t: ";
            cin.getline(buffer, MAX_STRING_LENGTH);
            newCar.rentFee = (strlen(buffer) == 0) ? showCar.rentFee : (int)atoi(buffer);

            strncpy(newCar.status, showCar.status, MAX_STRING_LENGTH);
            newCar.customerId = showCar.customerId;
            strncpy(newCar.customerName, showCar.customerName, MAX_STRING_LENGTH);
            newCar.rentDuration = showCar.rentDuration;
            newCar.rentDate = showCar.rentDate;

            writeData(data, pos, newCar);
            cout << "================================================" << endl;
            cout << GREEN << ITALIC << "Car " << regNumber << " successfully updated!" << RESET << endl;
        }
        else if (strcmp(showCar.status, "Rented") == 0)
        {
            cout << RED << ITALIC << "Cannot change the data of active rented car!" << RESET << endl;
            cin.ignore();
        }
    }
    else
    {
        cout << RED << ITALIC << "Car not found!" << RESET << endl;
        cin.ignore();
    }
    cin.get();
}

void removeCarList(fstream &data)
{
    system("cls");

    int size = getDataSize(data);
    cout << BG_WHITE << BLACK << "================== REMOVE CAR ==================" << RESET << endl;

    char regNumber[MAX_STRING_LENGTH];
    cout << "Registration Number: ";
    cin >> regNumber;
    toUpper(regNumber);
    cin.ignore();
    int pos = findPos(data, regNumber);

    if (pos >= 0)
    {
        CarRentData showCar = readData(data, pos);
        system("cls");
        cout << BG_BLUE << WHITE "=================== CAR DATA ===================" << RESET << endl;
        cout << "- Reg. Number\t\t: " << showCar.regNumber << endl;
        cout << "- Brand\t\t\t: " << showCar.brand << endl;
        cout << "- Model\t\t\t: " << showCar.model << endl;
        cout << "- Year\t\t\t: " << showCar.year << endl;
        cout << "- Rent Fee (/day)\t: Rp" << showCar.rentFee << endl;
        cout << "- Status\t\t: " << showCar.status << endl;

        char confirm;
        cout << "================================================" << endl;
        cout << "Remove this car? (y/n): ";
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y')
        {
            fstream temp;
            temp.open("temp.bin", ios::out | ios::binary);

            if (!temp.is_open())
            {
                cout << RED << ITALIC << "Error when creating temporary file" << RESET << endl;
                cout << GRAY << ITALIC << "Please try again!" << RESET << endl;

                cin.get();
                return;
            }

            for (int i = 1; i <= size; i++)
            {
                CarRentData car = readData(data, i);
                if (i != pos)
                    temp.write(reinterpret_cast<char *>(&car), sizeof(CarRentData));
            }

            data.close();
            remove(CARS_DATA_FILE);
            temp.close();
            rename("temp.bin", CARS_DATA_FILE);
            cout << GREEN << ITALIC << "Car " << regNumber << " successfully removed!" << RESET << endl;
            data.open(CARS_DATA_FILE, ios::out | ios::in | ios::binary);
        }
        else
        {
            cout << RED << ITALIC << "Car remove cancelled!" << RESET << endl;
        }
        cin.ignore();
        cin.get();
    }
    else
    {
        cout << RED << ITALIC << "Car not found!" << RESET << endl;
        cin.get();
    }
}

void rentCar(fstream &data)
{
    system("cls");
    int size = getDataSize(data);
    CarRentData rentCar;

    cout << BG_WHITE << BLACK << "=================== RENT CAR ===================" << RESET << endl;

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
                    cout << RED << ITALIC << "The amount paid is less than the total fee!" << RESET << endl;
                    cout << GRAY << ITALIC << "[ESC] to back to main menu" << RESET << endl;
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

            tm *rentDate = localtime(&rentCar.rentDate);
            char sRentDate[80];
            strftime(sRentDate, sizeof(sRentDate), "%Y-%m-%d %H:%M:%S", rentDate);

            updateHistory(rentCar, sRentDate, "rent");
            cin.ignore();
            cin.get();
        }
        else if (strcmp(rentCar.status, "Rented") == 0)
        {
            cout << RED << ITALIC << "Car " << regNumber << " is already rented!" << RESET << endl;
            cin.get();
        }
    }
    else
    {
        cout << RED << ITALIC << "Car not found!" << RESET << endl;
        cin.get();
        return;
    }
}

void returnCar(fstream &data)
{
    system("cls");
    int size = getDataSize(data);

    cout << BLACK << BG_WHITE << BOLD << "================== RETURN CAR ==================" << RESET << endl;

    char regNumber[MAX_STRING_LENGTH];
    cout << "Registration Number\t: ";
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
            cout << RED << ITALIC << "Car " << regNumber << " is not rented!" << RESET << endl;
            cin.get();
            return;
        }
    }
    else
    {
        cout << RED << ITALIC << "Car not found!" << RESET << endl;
        cin.get();
        return;
    }

    int customerId;
    cout << "Customer Id\t\t: ";
    cin >> customerId;

    char confirm;
    time_t now = time(0);
    tm *returnDate = localtime(&now);
    char sReturnDate[80];
    strftime(sReturnDate, sizeof(sReturnDate), "%Y-%m-%d %H:%M:%S", returnDate);

    cin.ignore();
    if (returnCar.customerId == customerId)
    {

        int actualRentDuration = difftime(now, returnCar.rentDate) / (60 * 60 * 24);
        int penaltyFee = (actualRentDuration - returnCar.rentDuration) * penaltyPercentage * returnCar.rentFee;

        makeInvoice(data, returnCar, "return", now);
        if (penaltyFee > 0)
        {
            while (true)
            {
                cout << "Actual Duration\t: " << actualRentDuration << " days" << endl;
                cout << "Penalty fee\t: Rp" << penaltyFee << endl;
                int payment;
                cout << "Payment\t\t: Rp";
                cin >> payment;
                if (payment >= penaltyFee)
                {
                    int refund = payment - penaltyFee;
                    cout << "Refund\t\t: Rp" << refund << endl;
                    break;
                }
                else
                {
                    cout << RED << ITALIC << "The amount paid is less than the penalty fee!" << RESET << endl;
                    cout << GRAY << ITALIC << "[ESC] to back to main menu" << RESET << endl;
                    cin.ignore();
                    makeInvoice(data, returnCar, "return", now);
                    if (_getch() == 27)
                        return;
                }
            }
        }
        else
        {
            while (true)
            {
                cout << "Penalty fee\t: Rp0" << endl;
                cout << "Are you sure you want to return this car? (y/n): ";
                cin >> confirm;
                if (confirm == 'Y' || confirm == 'y')
                {
                    break;
                }
                else if (confirm == 'N' || confirm == 'n')
                {
                    return;
                }
                else
                {
                    cout << RED << ITALIC << "Invalid input!" << RESET << endl;
                    cout << GRAY << ITALIC << "(ESC to back to main menu)" << RESET << endl;
                    cin.ignore();
                    makeInvoice(data, returnCar, "return", now);
                    if (_getch() == 27)
                        return;
                }
            }
        }
        strncpy(returnCar.status, "Not Rented", MAX_STRING_LENGTH);
        writeData(data, pos, returnCar);
        cout << "================== THANK YOU! ==================" << endl;
        cout << GREEN << ITALIC << "Car " << regNumber << " has been successfully returned!" << RESET << endl;

        updateHistory(returnCar, sReturnDate, "return");
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
    else
    {
        cout << RED << ITALIC << "Invalid customer id!" << RESET << endl;
        cin.get();
        return;
    }
}

int getChoice()
{
    cout << BG_WHITE << BLACK << "============== CAR RENT SERVICES ==============" << RESET << endl;
    cout << "1. Show Car List" << endl;
    cout << "2. Add Car List" << endl;
    cout << "3. Edit car" << endl;
    cout << "4. Remove Car On List" << endl;
    cout << "5. Rent Car" << endl;
    cout << "6. Return Car" << endl;
    cout << "===============================================" << endl;
    cout << "Choose menu [0 to exit]: ";
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
        cout << GREEN << "Database found!" << RESET << endl;
    }
    else
    {
        cout << RED << "Database not found! Creating new one..." << RED << endl;
        data.close();
        data.open(CARS_DATA_FILE, ios::trunc | ios::out | ios::in | ios::binary);
    }
    cout << GRAY << ITALIC << "[enter] to continue! " << RESET;
    cin.get();
}

void updateHistory(CarRentData &car, char time[], string action)
{
    ofstream history;
    history.open(HISTORY_FILE, ios::out | ios::app);
    if (!history.is_open())
    {
        history.close();
        history.open(HISTORY_FILE, ios::trunc | ios::out | ios::app);
    }

    history << time << '\t' << action << '\t' << car.regNumber << '\t' << car.customerName << '\t' << car.customerId << endl;
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

int getDataSize(fstream &data)
{
    int start, end;
    data.seekg(0, ios::beg);
    start = data.tellg();
    data.seekg(0, ios::end);
    end = data.tellg();
    return (end - start) / sizeof(CarRentData);
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
        cout << BG_WHITE << BLACK;
        cout << "================================================" << endl;
        cout << "                CAR RENT INVOICE                " << endl;
        cout << "================================================" << endl;
        cout << RESET;
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

        cout << BG_WHITE << BLACK;
        cout << "================================================" << endl;
        cout << "                CAR RETURN INVOICE              " << endl;
        cout << "================================================" << endl;
        cout << RESET;
        cout << "Customer\t: " << car.customerName << endl;
        cout << "Customer ID\t: " << car.customerId << endl;
        cout << "Reg. Number\t: " << car.regNumber << endl;
        cout << "Brand\t\t: " << car.brand << endl;
        cout << "Model\t\t: " << car.model << endl;
        cout << "Rent Duration\t: " << car.rentDuration << " days" << endl;
        cout << "Rent Date\t: " << sRentDate << endl;
        cout << "Return Date\t: " << sReturnDate << endl;
        cout << "================================================" << endl;
    }
}
