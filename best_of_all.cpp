#include <iostream>
#include <thread>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include "basic.h"
#include "dma.h"
#include "dma++.h"
#include "macd.h"
#include "rsi.h"
#include "adx.h"
#include "linear_regression.h"
using namespace std;

void strategy1(void* arg)
{   
    string temps1 = "7";
    string temps2 = "5";
    const char* argv[] = {temps1.c_str(), temps2.c_str()};
    basic(3, const_cast<char**>(argv), "basic_data.csv", "basic_order_statistics.csv", "basic_daily_cashflow.csv", "basic_final_pnl.txt");
    return;
}
void strategy2(void* arg)
{
    string temps1 = "50";
    string temps2 = "5";
    string temps3 = "2";
    const char* argv[] = {temps1.c_str(), temps2.c_str(), temps3.c_str()};
    dma(4, const_cast<char**>(argv), "dma_data.csv", "dma_order_statistics.csv", "dma_daily_cashflow.csv", "dma_final_pnl.txt");
    return;
}
void strategy3(void* arg)
{
    string temps1 = "14";
    string temps2 = "5";
    string temps3 = "5";
    string temps4 = "28";
    string temps5 = "2";
    string temps6 = "0.2";
    const char* argv[] = {temps1.c_str(), temps2.c_str(), temps3.c_str(), temps4.c_str(), temps5.c_str(), temps6.c_str()};
    dmapp(7, const_cast<char**>(argv), "dma++_data.csv", "dma++_order_statistics.csv", "dma++_daily_cashflow.csv", "dma++_final_pnl.txt");
}
void strategy4(void* arg)
{
    string temps1 = "5";
    const char* argv[] = {temps1.c_str()};
    macd(2, const_cast<char**>(argv), "macd_data.csv", "macd_order_statistics.csv", "macd_daily_cashflow.csv", "macd_final_pnl.txt");\
}
void strategy5(void* arg)
{
    string temps1 = "14";
    string temps2 = "5";
    string temps3 = "30";
    string temps4 = "70";
    const char* argv[] = {temps1.c_str(), temps2.c_str(), temps3.c_str(), temps4.c_str()};
    rsi(5, const_cast<char**>(argv), "rsi_data.csv", "rsi_order_statistics.csv", "rsi_daily_cashflow.csv", "rsi_final_pnl.txt");
}
void strategy6(void* arg)
{
    string temps1 = "14";
    string temps2 = "5";
    string temps3 = "25";
    const char* argv[] = {temps1.c_str(), temps2.c_str(), temps3.c_str()};
    adx(4, const_cast<char**>(argv), "adx_data.csv", "adx_order_statistics.csv", "adx_daily_cashflow.csv", "adx_final_pnl.txt");
}
void strategy7(void* arg)
{
    string temps1 = "5";
    string temps2 = "2";
    const char* argv[] = {temps1.c_str(), temps2.c_str()};
    lr(2, const_cast<char**>(argv), "lr_data.csv", "lr_order_statistics.csv", "lr_daily_cashflow.csv", "lr_final_pnl.txt");
}

int main(int argc, char *argv[])
{    
    thread thread1(strategy1, nullptr);
    thread thread2(strategy2, nullptr);
    thread thread3(strategy3, nullptr);
    thread thread4(strategy4, nullptr);
    thread thread5(strategy5, nullptr);
    thread thread6(strategy6, nullptr);
    thread thread7(strategy7, nullptr);

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();
    thread7.join();


    vector<string> strategies = {"basic", "dma", "dma++", "macd", "rsi", "adx", "lr"};

    int n = 0;
    double max_profit = -10000000.0f;

    for (int i = 0; i < 7; i++)
    {
        ifstream profits(strategies[i] + "_final_pnl" + ".txt");
        string line;
        getline(profits, line);
        double profit = stod(line);
        profits.close();
        if (profit > max_profit)
        {
            max_profit = profit;
            n = i;
        }
    }

    ofstream OrderFile("order_statistics.csv", ofstream::out | ofstream::trunc);
    ofstream CashFile("daily_cashflow.csv", ofstream::out | ofstream::trunc);
    ofstream ProfitFile("final_pnl.txt", ofstream::out | ofstream::trunc);

    ifstream iOrderFile(strategies[n] + "_order_statistics.csv");
    ifstream iCashFile(strategies[n] + "_daily_cashflow.csv");
    ifstream iProfitFile(strategies[n] + "_final_pnl.txt");

    OrderFile << iOrderFile.rdbuf();
    CashFile << iCashFile.rdbuf();
    ProfitFile << iProfitFile.rdbuf();

    iOrderFile.close();
    iCashFile.close();
    iProfitFile.close();

    for (int i = 0; i < 7; i++)
    {
        remove((strategies[i] + "_order_statistics.csv").c_str());
        remove((strategies[i] + "_daily_cashflow.csv").c_str());
        remove((strategies[i] + "_final_pnl.txt").c_str());
    }

    return 0;
}
