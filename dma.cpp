#include "dma.h"

int main(int argc, char *argv[])
{
    dma(argc, argv, "data.csv", "order_statistics.csv", "daily_cashflow.csv", "final_pnl.txt");
    return 0;
}