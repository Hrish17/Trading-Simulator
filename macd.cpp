#include "macd.h"

int main(int argc, char *argv[])
{
    macd(argc, argv, "data.csv", "order_statistics.csv", "daily_cashflow.csv", "final_pnl.txt");

    return 0;
}