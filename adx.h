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

struct AdxDate
{
    string day;
    string month;
    string year;
    double cprice;
    double lprice;
    double hprice;
    AdxDate(string d, string m, string y, double c, double l, double h)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
        lprice = l;
        hprice = h;
    }
};

string AdxFormatValue(double value)
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

void adx(int argc, char *argv[], string dataFile, string orderFile, string dailyFile, string finalFile)
{
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    double adx_threshold = stof(argv[3]);
    ifstream data(dataFile);

    string line;
    vector<AdxDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        istringstream iss(line);
        string token;
        getline(iss, token, ',');
        string day = token.substr(8, 2);
        string month = token.substr(5, 2);
        string year = token.substr(0, 4);
        getline(iss, token, ',');
        double cprice = stof(token);
        getline(iss, token, ',');
        double lprice = stof(token);
        getline(iss, token);
        double hprice = stof(token);
        dates.push_back(new AdxDate(day, month, year, cprice, lprice, hprice));
    }
    data.close();
    remove(dataFile.c_str());
    reverse(dates.begin(), dates.end());

    ofstream OrderFile(orderFile, ofstream::out | ofstream::trunc);
    ofstream CashFile(dailyFile, ofstream::out | ofstream::trunc);
    ofstream ProfitFile(finalFile, ofstream::out | ofstream::trunc);

    OrderFile << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    int NumOfStocks = 0;
    double Cash = 0;
    vector<double> TR(dates.size());
    vector<double> ATR(dates.size());
    vector<double> DMplus(dates.size());
    vector<double> DMminus(dates.size());
    vector<double> DIplusBase(dates.size());
    vector<double> DIminusBase(dates.size());
    vector<double> DIplus(dates.size());
    vector<double> DIminus(dates.size());
    vector<double> DX(dates.size());
    vector<double> ADX(dates.size());
    TR[1] = max(dates[1]->hprice - dates[1]->lprice, max(dates[1]->hprice - dates[0]->cprice, dates[1]->lprice - dates[0]->cprice));
    DMplus[1] = max(dates[1]->hprice - dates[0]->hprice, 0.0);
    DMminus[1] = max(dates[1]->lprice - dates[0]->lprice, 0.0);
    ATR[1] = TR[1];
    bool flag = false;
    if (ATR[1] == 0)
    {
        DIplusBase[1] = 0;
        DIminusBase[1] = 0;
        flag = true;
    }
    else
    {
        DIplusBase[1] = (DMplus[1]) / (ATR[1]);
        DIminusBase[1] = (DMminus[1]) / (ATR[1]);
    }
    DIplus[1] = DIplusBase[1];
    DIminus[1] = DIminusBase[1];
    if ((DIplus[1] + DIminus[1]) == 0)
    {
        DX[1] = 0;
        flag = true;
    }
    else
    {
        DX[1] = ((DIplus[1] - DIminus[1]) / (DIplus[1] + DIminus[1])) * (100.0);
    }
    ADX[1] = DX[1];
    if ((ADX[1] > adx_threshold) && (NumOfStocks < x) && (flag == false))
    {
        NumOfStocks++;
        Cash -= dates[1]->cprice;
        string CurrCash = AdxFormatValue(dates[1]->cprice);
        OrderFile << dates[1]->day << "/" << dates[1]->month << "/" << dates[1]->year << ",BUY,1," << CurrCash << endl;
    }
    else if ((ADX[1] < adx_threshold) && (NumOfStocks > -x) && (flag == false))
    {
        NumOfStocks--;
        Cash += dates[1]->cprice;
        string CurrCash = AdxFormatValue(dates[1]->cprice);
        OrderFile << dates[1]->day << "/" << dates[1]->month << "/" << dates[1]->year << ",SELL,1," << CurrCash << endl;
    }
    string CurrCash = AdxFormatValue(Cash);
    CashFile << dates[1]->day << "/" << dates[1]->month << "/" << dates[1]->year << "," << CurrCash << endl;
    for (int i = 2; i < dates.size(); i++)
    {
        flag = false;
        TR[i] = max(dates[i]->hprice - dates[i]->lprice, max(dates[i]->hprice - dates[i - 1]->cprice, dates[i]->lprice - dates[i - 1]->cprice));
        DMplus[i] = max(0.0, dates[i]->hprice - dates[i - 1]->hprice);
        DMminus[i] = max(0.0, dates[i]->lprice - dates[i - 1]->lprice);
        ATR[i] = (2.0 / (n + 1)) * (TR[i]) + (1 - (2.0 / (n + 1))) * (ATR[i - 1]);
        if (ATR[i] == 0)
        {
            DIplusBase[i] = 0;
            DIminusBase[i] = 0;
            flag = true;
        }
        else
        {
            DIplusBase[i] = (DMplus[i]) / (ATR[i]);
            DIminusBase[i] = (DMminus[i]) / (ATR[i]);
        }
        DIplus[i] = (2.0 / (n + 1)) * (DIplusBase[i]) + (1.0 - (2.0 / (n + 1))) * (DIplus[i - 1]);
        DIminus[i] = (2.0 / (n + 1)) * (DIminusBase[i]) + (1.0 - (2.0 / (n + 1))) * (DIminus[i - 1]);
        if ((DIplus[i] + DIminus[i]) == 0)
        {
            DX[i] = 0;
            flag = true;
        }
        else
        {
            DX[i] = ((DIplus[i] - DIminus[i]) / (DIplus[i] + DIminus[i])) * (100.0);
        }
        ADX[i] = (2.0 / (n + 1)) * (DX[i]) + (1 - (2.0 / (n + 1))) * (ADX[i - 1]);
        if ((ADX[i] > adx_threshold) && (NumOfStocks < x) && (flag == false))
        {
            NumOfStocks++;
            CurrCash = AdxFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
            Cash -= dates[i]->cprice;
        }
        else if ((ADX[i] < adx_threshold) && (NumOfStocks > -x) && (flag == false))
        {
            NumOfStocks--;
            CurrCash = AdxFormatValue(dates[i]->cprice);
            OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
            Cash += dates[i]->cprice;
        }
        CurrCash = AdxFormatValue(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
    }
    Cash += NumOfStocks * dates[dates.size() - 1]->cprice;
    OrderFile.close();
    CashFile.close();
    CurrCash = AdxFormatValue(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();
}
