#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <iomanip>
using namespace std;

struct DmaDate
{
    string day;
    string month;
    string year;
    double cprice;
    DmaDate(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

double sdfdma(double sum, double sumOfSquares, int n)
{
    return sqrt((sumOfSquares - (sum * sum) / n) / n);
}

string DmaFormatValue(double value)
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

void dma(int argc, char *argv[], string dataFile, string orderFile, string dailyFile, string finalFile)
{
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    double p = stof(argv[3]);
    ifstream data(dataFile);

    string line;
    vector<DmaDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates.push_back(new DmaDate(day, month, year, cprice));
    }
    data.close();
    remove(dataFile.c_str());
    reverse(dates.begin(), dates.end());

    ofstream OrderFile(orderFile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyFile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalFile, ofstream::out | ofstream::trunc);
    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    double sum = 0, sumOfSquares = 0;
    int NumOfStocks = 0;
    double Cash = 0;

    for (int i = 0; i < n; i++)
    {
        sum += dates[i]->cprice;
        sumOfSquares += dates[i]->cprice * dates[i]->cprice;
    }
    double dma = sum / n;
    double sd = sdfdma(sum, sumOfSquares, n);
    for (int i = n; i < dates.size(); i++)
    {
        sum += dates[i]->cprice - dates[i - n]->cprice;
        sumOfSquares += dates[i]->cprice * dates[i]->cprice - dates[i - n]->cprice * dates[i - n]->cprice;
        dma = sum / n;
        sd = sdfdma(sum, sumOfSquares, n);
        if (dates[i]->cprice >= dma + p * sd)
        {
            if (NumOfStocks < x)
            {
                NumOfStocks++;
                string CurrCash = DmaFormatValue(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
                Cash -= dates[i]->cprice;
            }
        }
        else if (dates[i]->cprice <= dma - p * sd)
        {
            if (NumOfStocks > -x)
            {
                NumOfStocks--;
                string CurrCash = DmaFormatValue(dates[i]->cprice);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
                Cash += dates[i]->cprice;
            }
        }
        string CurrCash = DmaFormatValue(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << fixed << setprecision(2) << CurrCash << endl;
    }

    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;
    OrderFile.close();
    CashFile.close();
    string CurrCash = DmaFormatValue(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();

    return;
}