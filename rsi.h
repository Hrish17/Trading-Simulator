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

struct RsiDate
{
    string day;
    string month;
    string year;
    double cprice;
    RsiDate(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

string RsiFormatValue(double value)
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

void rsi(int argc, char *argv[], string datafile, string orderfile, string dailyfile, string finalfile)
{
    int n = stoi(argv[1]);                       // no. of days
    int x = stoi(argv[2]);                       // limit of number of shares
    double oversold_threshold = stod(argv[3]);   // oversold threshold
    double overbought_threshold = stod(argv[4]); // overbought threshold
    ifstream data(datafile);

    string line;
    vector<RsiDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates.push_back(new RsiDate(day, month, year, cprice));
    }
    data.close();
    remove(datafile.c_str());
    reverse(dates.begin(), dates.end());
    ofstream OrderFile(orderfile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyfile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalfile, ofstream::out | ofstream::trunc);

    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    vector<double> GainSum(dates.size());
    vector<double> LossSum(dates.size());
    GainSum[0] = 0.0;
    LossSum[0] = 0.0;
    for (int i = 1; i < n; i++)
    {
        GainSum[i] = GainSum[i - 1] + max(dates[i]->cprice - dates[i - 1]->cprice, 0.0);
        LossSum[i] = LossSum[i - 1] + max(dates[i - 1]->cprice - dates[i]->cprice, 0.0);
    }
    int NumOfStocks = 0;
    double Cash = 0.0;
    for (int i = n; i < dates.size(); i++)
    {
        GainSum[i] = GainSum[i - 1] + max(dates[i]->cprice - dates[i - 1]->cprice, 0.0);
        LossSum[i] = LossSum[i - 1] + max(dates[i - 1]->cprice - dates[i]->cprice, 0.0);
        double AvgGain = (GainSum[i] - GainSum[i - n]) / n;
        double AvgLoss = (LossSum[i] - LossSum[i - n]) / n;
        double RS = AvgGain / AvgLoss;
        double RSI = 100.0 - (100.0 / (1.0 + RS));
        if ((RSI < oversold_threshold) && (NumOfStocks < x))
        {
            NumOfStocks++;
            string CurrCash = RsiFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
            Cash -= dates[i]->cprice;
        }
        else if ((RSI > overbought_threshold) && (NumOfStocks > -x))
        {
            NumOfStocks--;
            string CurrCash = RsiFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
            Cash += dates[i]->cprice;
        }
        string CurrCash = RsiFormatValue(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
    }
    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;
    OrderFile.close();
    CashFile.close();
    string CurrCash = RsiFormatValue(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();
}