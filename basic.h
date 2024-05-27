#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <cmath>
using namespace std;

struct BasicDate
{
    string day;
    string month;
    string year;
    double cprice;
    BasicDate(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

string BasicFormatValue(double value)
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

void basic(int argc, char *argv[], string dataFile, string orderFile, string dailyFile, string finalFile)
{
    int n = stoi(argv[1]); // no. of days
    int x = stoi(argv[2]); // limit of number shares value
    ifstream data(dataFile);
    string line;
    vector<BasicDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates.push_back(new BasicDate(day, month, year, cprice));
    }
    data.close();
    remove(dataFile.c_str());
    reverse(dates.begin(), dates.end());
    ofstream OrderFile(orderFile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyFile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalFile, ofstream::out | ofstream::trunc);
    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    int count1 = 0, count2 = 0; // count1 for increasing, count2 for decreasing
    int NumOfStocks = 0;
    double Cash = 0;
    for (int i = 1; i < dates.size(); i++)
    {
        if (dates[i]->cprice > dates[i - 1]->cprice)
        {
            count1++;
            count2 = 0;
            if ((count1 >= n) && (NumOfStocks < x))
            {
                NumOfStocks++;
                string CurrCash = BasicFormatValue(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
                Cash -= dates[i]->cprice;
            }
        }
        else if (dates[i]->cprice < dates[i - 1]->cprice)
        {
            count2++;
            count1 = 0;
            if ((count2 >= n) && (NumOfStocks > -x))
            {
                NumOfStocks--;
                string CurrCash = BasicFormatValue(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
                Cash += dates[i]->cprice;
            }
        }
        else
        {
            count1 = 0;
            count2 = 0;
        }
        if (i >= n)
        {
            string CurrCash = BasicFormatValue(Cash);
            CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
        }
    }
    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;
    OrderFile.close();
    CashFile.close();
    string CurrCash = BasicFormatValue(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();

    return;
}