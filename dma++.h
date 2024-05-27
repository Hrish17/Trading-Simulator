#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <queue>
#include <iomanip>
using namespace std;

struct DmappDate
{
    string day;
    string month;
    string year;
    double cprice;
    DmappDate(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

double sdf(double sum, double sumOfSquares, int n)
{
    return sqrt((sumOfSquares - (sum * sum) / n) / n);
}

string formatDoubleDMApp(double value)
{
    stringstream ss;
    if (abs(value - round(value)) < 0.01)
    {
        ss << fixed << setprecision(0) << value;
    }
    else
    {
        ss << fixed << setprecision(2) << value;
        string s = ss.str();
        size_t pos = s.find_last_not_of('0');
        if (pos != string::npos && s[pos] == '.')
        {
            pos++;
        }
        return s.substr(0, pos + 1);
    }
    return ss.str();
}

void dmapp(int argc, char *argv[], string dataFile, string orderFile, string dailyFile, string finalFile)
{
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    double p = stod(argv[3]);
    int maxHoldDays = stoi(argv[4]);
    double c1 = stod(argv[5]);
    double c2 = stod(argv[6]);
    ifstream data(dataFile);

    string line;
    vector<DmappDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stod(line.substr(11));
        dates.push_back(new DmappDate(day, month, year, cprice));
    }
    data.close();
    remove(dataFile.c_str());
    reverse(dates.begin(), dates.end());

    ofstream OrderFile(orderFile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyFile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalFile, ofstream::out | ofstream::trunc);
    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    double sumOfAbsoluteChanges = 0;
    double sf = 0.5;
    double ama = dates[n]->cprice;
    bool buy = false;
    queue<int> buyDates;
    int NumOfStocks = 0;
    double Cash = 0;

    for (int i = 0; i < n; i++)
    {
        sumOfAbsoluteChanges += abs(dates[i]->cprice - dates[i + 1]->cprice);
    }

    CashFile << dates[n]->day << "/" << dates[n]->month << "/" << dates[n]->year << "," << 0 << endl;

    for (int i = n + 1; i < dates.size(); i++)
    {
        int quantity = 0;
        double change = dates[i]->cprice - dates[i - n]->cprice;
        sumOfAbsoluteChanges = sumOfAbsoluteChanges - abs(dates[i - n - 1]->cprice - dates[i - n]->cprice) + abs(dates[i - 1]->cprice - dates[i]->cprice);
        bool flag = true;
        bool currentBuy;
        if (sumOfAbsoluteChanges > 0)
        {
            double er = change / sumOfAbsoluteChanges;
            sf = sf + c1 * (((2 * er) / (1 + c2) - 1) / ((2 * er) / (1 + c2) + 1) - sf);
            ama = ama + sf * (dates[i]->cprice - ama);
            if (dates[i]->cprice - ama >= ama * p / 100 && NumOfStocks < x)
            {
                quantity++;
                NumOfStocks++;
                currentBuy = true;
                Cash -= dates[i]->cprice;
                if (buy == false && buyDates.size() > 0)
                {
                    buyDates.pop();
                    flag = false;
                }
                else
                {
                    buy = true;
                    buyDates.push(i);
                }
            }
            else if (dates[i]->cprice - ama <= -ama * p / 100 && NumOfStocks > -x)
            {
                quantity++;
                NumOfStocks--;
                currentBuy = false;
                Cash += dates[i]->cprice;
                if (buy == true && buyDates.size() > 0)
                {
                    buyDates.pop();
                    flag = false;
                }
                else
                {
                    buy = false;
                    buyDates.push(i);
                }
            }
        }
        if (flag && buyDates.size() > 0)
        {
            if (i - buyDates.front() > maxHoldDays)
            {
                quantity++;
                buyDates.pop();
                if (buy)
                {
                    NumOfStocks--;
                    Cash += dates[i]->cprice;
                    currentBuy = false;
                }
                else
                {
                    NumOfStocks++;
                    Cash -= dates[i]->cprice;
                    currentBuy = true;
                }
            }
        }
        if (quantity > 0)
        {
            if (currentBuy)
            {
                string CurrCash = formatDoubleDMApp(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY," << quantity << "," << CurrCash << endl;
            }
            else
            {
                string CurrCash = formatDoubleDMApp(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL," << quantity << "," << CurrCash << endl;
            }
        }
        string CurrCash = formatDoubleDMApp(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
    }

    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;

    OrderFile.close();
    CashFile.close();
    string CurrCash = formatDoubleDMApp(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();
}