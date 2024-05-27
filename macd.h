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

struct MacdDate
{
    string day;
    string month;
    string year;
    double cprice;
    MacdDate(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

string MacdFormatValue(double value)
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

void macd(int argc, char *argv[], string datafile, string orderfile, string dailyfile, string finalfile)
{
    int x = stoi(argv[1]);
    ifstream data(datafile);
    string line;
    vector<MacdDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates.push_back(new MacdDate(day, month, year, cprice));
    }
    data.close();
    remove(datafile.c_str());
    reverse(dates.begin(), dates.end());

    ofstream OrderFile(orderfile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyfile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalfile, ofstream::out | ofstream::trunc);

    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    vector<double> ShortEwm(dates.size());
    vector<double> LongEwm(dates.size());
    vector<double> Macd(dates.size());
    vector<double> Signal(dates.size());
    ShortEwm[0] = dates[0]->cprice;
    LongEwm[0] = dates[0]->cprice;
    Macd[0] = 0;
    Signal[0] = 0;
    int NumOfStocks = 0;
    double Cash = 0;
    string CurrCash = MacdFormatValue(Cash);
    CashFile << dates[0]->day << "/" << dates[0]->month << "/" << dates[0]->year << "," << CurrCash << endl;
    for (int i = 1; i < dates.size(); i++)
    {
        ShortEwm[i] = (2.0 / 13) * dates[i]->cprice + (1 - (2.0 / 13)) * ShortEwm[i - 1];
        LongEwm[i] = (2.0 / 27) * dates[i]->cprice + (1 - (2.0 / 27)) * LongEwm[i - 1];
        Macd[i] = ShortEwm[i] - LongEwm[i];
        Signal[i] = (2.0 / 10) * Macd[i] + (1 - (2.0 / 10)) * Signal[i - 1];
        double CurrMacd = Macd[i];
        double CurrSignal = Signal[i];
        if ((CurrMacd > CurrSignal) && (NumOfStocks < x))
        {
            NumOfStocks++;
            CurrCash = MacdFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
            Cash -= dates[i]->cprice;
        }
        else if ((CurrMacd < CurrSignal) && (NumOfStocks > -x))
        {
            NumOfStocks--;
            CurrCash = MacdFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
            Cash += dates[i]->cprice;
        }
        CurrCash = MacdFormatValue(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
    }

    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;

    OrderFile.close();
    CashFile.close();
    CurrCash = MacdFormatValue(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();
}