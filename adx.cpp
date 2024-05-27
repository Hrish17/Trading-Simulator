#include "adx.h"

int main(int argc, char *argv[])
{
    adx(argc, argv, "data.csv", "order_statistics.csv", "daily_cashflow.csv", "final_pnl.txt");
    return 0;
}