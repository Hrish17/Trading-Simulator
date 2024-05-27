from datetime import datetime, timedelta
from jugaad_data import nse
import sys

def getStockData1(sym, n, start_date, end_date, filename):
    cnt = 0
    while (cnt < n):
        start_date = start_date - timedelta(days=1)
        try:
            temp_data = nse.stock_df(sym, start_date, start_date)
        except:
            continue
        cnt += 1
    stk_df = nse.stock_df(sym, start_date, end_date)
    stk_data = stk_df[["DATE", "CLOSE"]]
    stk_data.to_csv(filename, index=False)

def getStockData2(sym, n, start_date, end_date, filename):
    cnt = 0
    while (cnt < n):
        start_date = start_date - timedelta(days=1)
        try:
            temp_data = nse.stock_df(sym, start_date, start_date)
        except:
            continue
        cnt += 1
    stk_df = nse.stock_df(sym, start_date, end_date)
    stk_data = stk_df[["DATE", "CLOSE", "LOW", "HIGH",]]
    stk_data.to_csv(filename, index=False)

def getStockData3(sym, start_date, end_date):
    n = 1
    cnt = 0
    while (cnt < n):
        start_date = start_date - timedelta(days=1)
        try:
            temp_data = nse.stock_df(sym, start_date, start_date)
        except:
            continue
        cnt += 1
    stk_df = nse.stock_df(sym, start_date, end_date)
    stk_data = stk_df[["DATE", "CLOSE", "OPEN", "VWAP", "LOW", "HIGH", "NO OF TRADES"]]
    return stk_data

if __name__ == "__main__":
    option = int(sys.argv[1])
    if(option == 1):
        sym = sys.argv[2]
        n = int(sys.argv[3])
        start_date = datetime.strptime(sys.argv[4], "%d/%m/%Y").date()
        end_date = datetime.strptime(sys.argv[5], "%d/%m/%Y").date()
        getStockData1(sym, n, start_date, end_date, "data.csv")
        
    elif(option == 2):
        sym = sys.argv[2]
        n = int(sys.argv[3])
        start_date = datetime.strptime(sys.argv[4], "%d/%m/%Y").date()
        end_date = datetime.strptime(sys.argv[5], "%d/%m/%Y").date()
        getStockData2(sym, n, start_date, end_date, "data.csv")

    elif(option == 3):
        sym = sys.argv[2]
        train_start_date = datetime.strptime(sys.argv[3], "%d/%m/%Y").date()
        train_end_date = datetime.strptime(sys.argv[4], "%d/%m/%Y").date()
        start_date = datetime.strptime(sys.argv[5], "%d/%m/%Y").date()
        end_date = datetime.strptime(sys.argv[6], "%d/%m/%Y").date()
        stkdata = getStockData3(sym, start_date, end_date)
        stkdata.to_csv("data.csv", index=False)
        train_stkdata = getStockData3(sym, train_start_date, train_end_date)
        train_stkdata.to_csv("train_data.csv", index=False)

    elif(option == 4):
        sym1 = sys.argv[2]
        sym2 = sys.argv[3]
        n = int(sys.argv[4])
        start_date = datetime.strptime(sys.argv[5], "%d/%m/%Y").date()
        end_date = datetime.strptime(sys.argv[6], "%d/%m/%Y").date()
        getStockData1(sym1, n, start_date, end_date, "data1.csv")
        getStockData1(sym2, n, start_date, end_date, "data2.csv")

    elif(option == 5):
        sym = sys.argv[2]
        start_date = datetime.strptime(sys.argv[3], "%d/%m/%Y").date()
        end_date = datetime.strptime(sys.argv[4], "%d/%m/%Y").date()
        train_start_date = start_date - timedelta(days=365)
        train_end_date = end_date - timedelta(days=365)
        getStockData1(sym, 7, start_date, end_date, "basic_data.csv")
        getStockData1(sym, 50, start_date, end_date, "dma_data.csv")
        getStockData1(sym, 14, start_date, end_date, "dma++_data.csv")
        getStockData1(sym, 0, start_date, end_date, "macd_data.csv")
        getStockData1(sym, 14, start_date, end_date, "rsi_data.csv")
        getStockData2(sym, 1, start_date, end_date, "adx_data.csv")
        getStockData3(sym, train_start_date, train_end_date).to_csv("train_data.csv", index=False)
        getStockData3(sym, start_date, end_date).to_csv("lr_data.csv", index=False)