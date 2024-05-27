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

struct Date
{
    string day;
    string month;
    string year;
    double cprice;
    Date(string d, string m, string y, double c)
    {
        day = d;
        month = m;
        year = y;
        cprice = c;
    }
};

double sdf(double sum, double sumOfSquares, double n)
{
    return sqrt((sumOfSquares - (sum * sum) / n) / n);
}

string formatDouble(double value)
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

int main(int argc, char *argv[])
{
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    int threshold = stoi(argv[3]);
    int stop_loss_threshold = stoi(argv[4]);

    double n1 = (double)n;

    ifstream data1("data1.csv");

    string line;
    vector<Date *> dates1;
    getline(data1, line);
    while (getline(data1, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates1.push_back(new Date(day, month, year, cprice));
    }
    data1.close();
    remove("data1.csv");
    reverse(dates1.begin(), dates1.end());

    ifstream data2("data2.csv");

    vector<Date *> dates2;
    getline(data2, line);
    while (getline(data2, line))
    {
        string day = line.substr(8, 2);
        string month = line.substr(5, 2);
        string year = line.substr(0, 4);
        double cprice = stof(line.substr(11));
        dates2.push_back(new Date(day, month, year, cprice));
    }
    data2.close();
    remove("data2.csv");
    reverse(dates2.begin(), dates2.end());

    ofstream OrderFile1("order_statistics_1.csv", ofstream::out | ofstream::trunc);
    ofstream OrderFile2("order_statistics_2.csv", ofstream::out | ofstream::trunc);
    ofstream CashFile("daily_cashflow.csv", ofstream::out | ofstream::trunc);
    OrderFile1 << "Date,Order_dir,Quantity,Price" << endl;
    OrderFile2 << "Date,Order_dir,Quantity,Price" << endl;
    CashFile << "Date,Cashflow" << endl;
    double sum = 0, sumOfSquares = 0;
    double Cash = 0;
    int NumOfStocks = 0;

    for (int i = 0; i < n; i++)
    {
        double spread = dates1[i]->cprice - dates2[i]->cprice;
        sum += spread;
        sumOfSquares += spread * spread;
    }

    double rollingMean;
    double rollingSD;

    vector<pair<double, double>> meansAndSDs;
    bool pos = false;

    for (int i = n; i < dates1.size(); i++)
    {
        int quantity = 0;
        double prevSpread = dates1[i - n]->cprice - dates2[i - n]->cprice;
        double spread = dates1[i]->cprice - dates2[i]->cprice;
        sum += spread - prevSpread;
        sumOfSquares += spread * spread - prevSpread * prevSpread;
        rollingMean = sum / n1;
        rollingSD = sdf(sum, sumOfSquares, n1);
        double zScore = (spread - rollingMean) / rollingSD;
        if (zScore > threshold)
        {
            if (NumOfStocks < x)
            {
                NumOfStocks++;
                quantity++;
                Cash += spread;
                if (pos == false && meansAndSDs.size() > 0)
                {
                    meansAndSDs.erase(meansAndSDs.begin());
                }
                else
                {
                    meansAndSDs.push_back(make_pair(rollingMean, rollingSD));
                    pos = true;
                }
            }
        }
        else if (zScore < -threshold)
        {
            if (NumOfStocks > -x)
            {
                NumOfStocks--;
                quantity--;
                Cash -= spread;
                if (pos == true && meansAndSDs.size() > 0)
                {
                    meansAndSDs.erase(meansAndSDs.begin());
                }
                else
                {
                    meansAndSDs.push_back(make_pair(rollingMean, rollingSD));
                    pos = false;
                }
            }
        }
        auto it = meansAndSDs.begin();
        while (it != meansAndSDs.end())
        {
            double tempMean = it->first;
            double tempSD = it->second;
            double tempZScore = (spread - tempMean) / tempSD;
            if (abs(tempZScore) > stop_loss_threshold)
            {
                meansAndSDs.erase(it);
                if (pos)
                {
                    NumOfStocks--;
                    quantity--;
                    Cash -= spread;
                }
                else
                {
                    NumOfStocks++;
                    quantity++;
                    Cash += spread;
                }
            }
            else
            {
                it++;
            }
        }
        if (quantity > 0)
        {
            string CurrCash1 = formatDouble(dates1[i]->cprice);
            string CurrCash2 = formatDouble(dates2[i]->cprice);
            OrderFile1 << dates1[i]->day << "/" << dates1[i]->month << "/" << dates1[i]->year << ",SELL," << quantity << "," << CurrCash1 << endl;
            OrderFile2 << dates2[i]->day << "/" << dates2[i]->month << "/" << dates2[i]->year << ",BUY," << quantity << "," << CurrCash2 << endl;
        }
        else if (quantity < 0)
        {
            string CurrCash1 = formatDouble(dates1[i]->cprice);
            string CurrCash2 = formatDouble(dates2[i]->cprice);
            OrderFile1 << dates1[i]->day << "/" << dates1[i]->month << "/" << dates1[i]->year << ",BUY," << -quantity << "," << CurrCash1 << endl;
            OrderFile2 << dates2[i]->day << "/" << dates2[i]->month << "/" << dates2[i]->year << ",SELL," << -quantity << "," << CurrCash2 << endl;
        }
        string CurrCash = formatDouble(Cash);
        CashFile << dates1[i]->day << "/" << dates1[i]->month << "/" << dates1[i]->year << "," << CurrCash << endl;
    }

    Cash -= NumOfStocks * (dates1[dates1.size() - 1]->cprice - dates2[dates2.size() - 1]->cprice);

    OrderFile1.close();
    OrderFile2.close();
    CashFile.close();
    ofstream ProfitFile("final_pnl.txt", ofstream::out | ofstream::trunc);
    string CurrCash = formatDouble(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();

    return 0;
}