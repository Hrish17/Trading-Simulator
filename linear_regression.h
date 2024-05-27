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

struct LrDate
{
    string day;
    string month;
    string year;
    double close;
    double open;
    double vwap;
    double lprice;
    double hprice;
    double trades;
    LrDate(string d, string m, string y, double c, double o, double v, double l, double h, double t)
    {
        day = d;
        month = m;
        year = y;
        close = c;
        open = o;
        vwap = v;
        lprice = l;
        hprice = h;
        trades = t;
    }
};

vector<vector<double>> transpose(vector<vector<double>> X)
{
    vector<vector<double>> XT;
    for (int i = 0; i < X[0].size(); i++)
    {
        vector<double> tmp;
        for (int j = 0; j < X.size(); j++)
        {
            tmp.push_back(X[j][i]);
        }
        XT.push_back(tmp);
    }
    return XT;
}

vector<vector<double>> inverse(vector<vector<double>> X)
{
    vector<vector<double>> I;
    for (int i = 0; i < X.size(); i++)
    {
        vector<double> tmp;
        for (int j = 0; j < X[0].size(); j++)
        {
            if (i == j)
            {
                tmp.push_back(1);
            }
            else
            {
                tmp.push_back(0);
            }
        }
        I.push_back(tmp);
    }
    for (int i = 0; i < X.size(); i++)
    {
        double divisor = X[i][i];
        for (int j = 0; j < X[0].size(); j++)
        {
            X[i][j] /= divisor;
            I[i][j] /= divisor;
        }
        for (int j = 0; j < X.size(); j++)
        {
            if (i != j)
            {
                double multiplier = X[j][i];
                for (int k = 0; k < X[0].size(); k++)
                {
                    X[j][k] -= X[i][k] * multiplier;
                    I[j][k] -= I[i][k] * multiplier;
                }
            }
        }
    }
    return I;
}

vector<vector<double>> matrixMultiplication(vector<vector<double>> X, vector<vector<double>> Y)
{
    vector<vector<double>> result;
    for (int i = 0; i < X.size(); i++)
    {
        vector<double> tmp;
        for (int j = 0; j < Y[0].size(); j++)
        {
            double sum = 0;
            for (int k = 0; k < X[0].size(); k++)
            {
                sum += X[i][k] * Y[k][j];
            }
            tmp.push_back(sum);
        }
        result.push_back(tmp);
    }
    return result;
}

vector<double> train(vector<LrDate *> train_dates)
{
    vector<vector<double>> X;
    for (int i = 1; i < train_dates.size(); i++)
    {
        vector<double> tmp;
        tmp.push_back(1);
        tmp.push_back(train_dates[i - 1]->close);
        tmp.push_back(train_dates[i - 1]->open);
        tmp.push_back(train_dates[i - 1]->vwap);
        tmp.push_back(train_dates[i - 1]->lprice);
        tmp.push_back(train_dates[i - 1]->hprice);
        tmp.push_back(train_dates[i - 1]->trades);
        tmp.push_back(train_dates[i]->open);
        X.push_back(tmp);
    }
    vector<vector<double>> y;
    for (int i = 1; i < train_dates.size(); i++)
    {
        vector<double> tmp;
        tmp.push_back(train_dates[i]->close);
        y.push_back(tmp);
    }
    vector<vector<double>> XT = transpose(X);
    vector<vector<double>> XTX = matrixMultiplication(XT, X);
    vector<vector<double>> XTXI = inverse(XTX);
    vector<vector<double>> XTy = matrixMultiplication(XT, y);
    vector<vector<double>> weights = matrixMultiplication(XTXI, XTy);
    vector<double> result;
    for (int i = 0; i < weights.size(); i++)
    {
        result.push_back(weights[i][0]);
    }
    return result;
}

string formatDoubleLR(double value)
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

void lr(int argc, char *argv[], string dataFile, string orderFile, string dailyFile, string finalFile)
{
    int x = stoi(argv[1]);
    double p = stod(argv[2]);

    ifstream train_data("train_data.csv");

    string line;
    vector<LrDate *> train_dates;
    getline(train_data, line);
    while (getline(train_data, line))
    {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ','))
        {
            tokens.push_back(token);
        }
        string day = tokens[0].substr(8, 2);
        string month = tokens[0].substr(5, 2);
        string year = tokens[0].substr(0, 4);
        double close = stod(tokens[1]);
        double open = stod(tokens[2]);
        double vwap = stod(tokens[3]);
        double lprice = stod(tokens[4]);
        double hprice = stod(tokens[5]);
        double trades = stod(tokens[6]);
        train_dates.push_back(new LrDate(day, month, year, close, open, vwap, lprice, hprice, trades));
    }
    train_data.close();
    remove("train_data.csv");
    reverse(train_dates.begin(), train_dates.end());

    vector<double> weights = train(train_dates);

    ifstream data(dataFile);
    vector<LrDate *> dates;
    getline(data, line);
    while (getline(data, line))
    {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ','))
        {
            tokens.push_back(token);
        }
        string day = tokens[0].substr(8, 2);
        string month = tokens[0].substr(5, 2);
        string year = tokens[0].substr(0, 4);
        double close = stod(tokens[1]);
        double open = stod(tokens[2]);
        double vwap = stod(tokens[3]);
        double lprice = stod(tokens[4]);
        double hprice = stod(tokens[5]);
        double trades = stod(tokens[6]);
        dates.push_back(new LrDate(day, month, year, close, open, vwap, lprice, hprice, trades));
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

    for (int i = 1; i < dates.size(); i++)
    {
        double prediction = weights[0] + weights[1] * dates[i - 1]->close + weights[2] * dates[i - 1]->open + weights[3] * dates[i - 1]->vwap + weights[4] * dates[i - 1]->lprice + weights[5] * dates[i - 1]->hprice + weights[6] * dates[i - 1]->trades + weights[7] * dates[i]->open;
        if (prediction >= dates[i]->close * (1 + p / 100))
        {
            if (NumOfStocks < x)
            {
                NumOfStocks++;
                string CurrCash = formatDoubleLR(dates[i]->close);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",BUY,1," << CurrCash << endl;
                Cash -= dates[i]->close;
            }
        }
        else if (prediction <= dates[i]->close * (1 - p / 100))
        {
            if (NumOfStocks > -x)
            {
                NumOfStocks--;
                string CurrCash = formatDoubleLR(dates[i]->close);
                OrderFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << ",SELL,1," << CurrCash << endl;
                Cash += dates[i]->close;
            }
        }
        string CurrCash = formatDoubleLR(Cash);
        CashFile << dates[i]->day << "/" << dates[i]->month << "/" << dates[i]->year << "," << CurrCash << endl;
    }

    Cash += NumOfStocks * dates[dates.size() - 1]->close;

    OrderFile.close();
    CashFile.close();
    string CurrCash = formatDoubleLR(Cash);
    ProfitFile << CurrCash << endl;
    ProfitFile.close();
}