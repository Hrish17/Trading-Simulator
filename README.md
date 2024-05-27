## Trading Simulator:

### Basic Strategy :

- The basic strategy observes the price series for the past n days. If the price has been monotonically increasing for the last n days, it buys 1 share; if it has been monotonically decreasing, it sells 1 share.
- Buy signals are generated when the price has been increasing for the last n days, and the number of shares is within the limit.
- Sell signals are generated when the price has been decreasing for the last n days, and the number of shares is within the limit.
- The implementation uses a linear scan over the historical data, resulting in a time complexity of O(N), where N is the number of data points.

### n-Day Moving Average (DMA) Strategy :

- The DMA strategy involves calculating two moving averages of the stock price over different periods and generating buy/sell signals based on their crossovers.
- Buy signals are generated when the current price crosses above the moving average plus a multiple of the standard deviation.
- Sell signals are generated when the current price crosses below the moving average minus a multiple of the standard deviation.
- The implementation uses a linear scan over the historical data to calculate moving averages and standard deviations, resulting in a time complexity of O(N), where N is the number of data points.

### Improved DMA Strategy :

- The DMApp strategy is an extension of the DMA strategy, incorporating percentage-based thresholds for buy/sell signals in addition to dual moving averages.
- Buy signals are generated when the current price crosses above the adjusted moving average by a certain percentage and vice versa for sell signals.
- Additionally, a maximum holding period is enforced, and stocks are sold if this period is exceeded.
- Orders are written to the order statistics file, and daily cash flow is updated accordingly.
- Similar to the DMA strategy, the DMApp strategy employs a linear scan over the historical data to calculate moving averages and adjust them based on percentage thresholds, resulting in a time complexity of O(N), where N is the number of data points.
- The use of a queue to track buy dates enables efficient tracking of the maximum holding period for stocks.

### Moving Average Convergence Divergence (MACD) Strategy :

- The MACD strategy is a popular trend-following strategy that utilizes the moving average convergence divergence indicator to generate buy and sell signals.
- It calculates the short and long exponential moving averages (EMA) along with the MACD line and the signal line.
- Buy signals are generated when the MACD line crosses above the signal line, and sell signals occur when the MACD line crosses below the signal line.
- The MACD strategy employs a linear scan over the historical data to calculate exponential moving averages and MACD lines, resulting in a time complexity of O(N), where N is the number of data points.

### Relative Strength Index (RSI) Strategy :

- The RSI strategy is a momentum oscillator that measures the speed and change of price movements. It identifies overbought or oversold conditions in the trading of an asset.
- It calculates the relative strength (RS) and the RSI indicator based on the given number of days (n).
- Buy signals are generated when the RSI falls below the oversold threshold, and sell signals occur when the RSI rises above the overbought threshold.
- The RSI strategy utilizes a linear scan over the historical data to calculate the RSI indicator, resulting in a time complexity of O(N), where N is the number of data points.

### Average Directional Index (ADX) Strategy :

- The ADX strategy is a trend-following indicator that measures the strength of a trend. It aims to identify whether the market is trending or ranging.
- It calculates various components of the ADX indicator, including True Range (TR), Average True Range (ATR), Positive Directional Movement (+DM), Negative Directional Movement (-DM), Directional Indexes (DI), and the ADX itself.
- Buy signals are generated when the ADX rises above the specified threshold, indicating a strengthening trend, and sell signals occur when the ADX falls below the threshold, suggesting a weakening trend.
- The ADX strategy utilizes a linear scan over the historical data to calculate various components of the ADX indicator, resulting in a time complexity of O(N), where N is the number of data points.

### Linear Regression (LR) Strategy :

- The LR strategy utilizes a linear regression model to predict future stock prices based on historical data. It makes buy or sell decisions based on the predicted prices relative to certain thresholds.
- Historical stock data is used to train a linear regression model.
- The trained model is applied to predict the next closing price using the features of the current day.
- If the predicted price is above a certain percentage threshold (`p`), a buy signal is generated if the number of stocks (`x`) held is less than a specified limit.
- If the predicted price is below the threshold, a sell signal is generated if the number of stocks held is greater than the negative of the limit (`-x`).
- The LR strategy involves training a linear regression model, which typically requires computing matrix operations like matrix multiplication and inversion.
- Efficient algorithms for matrix operations and linear regression training can optimize the time complexity of the LR strategy.

### Best of All Strategy :

- The multi-strategy trading application implements concurrent execution of multiple trading strategies and selects the one with the highest profit for further analysis. It utilizes multithreading to execute each strategy independently and aggregates the results to determine the most profitable strategy.
- Strategies used - Basic, DMA, DMA++, MACD, RSI, ADX, LR.
- Each strategy is executed concurrently using separate threads, allowing for parallel computation and faster processing.
- Upon completion, the application selects the strategy with the highest profit by comparing the final profit values obtained from each strategy's output files.
- Enhance the application to support dynamic addition and removal of trading strategies without requiring code modifications.

### Mean-Reverting Pairs Trading Strategy (Part 4.0) :

- Pairs trading is a popular trading strategy that involves taking long and short positions in two related assets. The strategy exploits temporary divergences in the prices of the two assets, aiming to profit from mean reversion.
- Buy signals are generated when the spread deviates below a lower threshold, indicating that one asset is relatively cheaper compared to the other, and sell signals occur when the spread exceeds an upper threshold, suggesting overvaluation.
- The implementation uses a linear scan over historical data to calculate the spread and generate trading signals, resulting in a time complexity of O(N), where N is the number of data points.

## Our Approach for Pairs Trading with Stop Loss Strategy (Part 4.1):

- For each day, We're maintaining an integer quantity that that stores the amount of positions to be bought or sold. It will be equal to difference between SELL signal and BUY signal for that particular day. The quantity will depend upon signals which will come both from the threshold part and stop-loss part.
- First, We're checking for the threshold part which is almost same as the plain pairs strategy. The only thing we did extra is that we're maintaining a vector meanAndSDs which contains the positions that we're currently holding. At a time, meanAndSDs will either contain positions of BUY signals or positions of SELL signals.
- Then, we perform the stop-loss part. In this, we are checking for all the positions that we are currently holding. We calculate the z-score using the spread according to today's prices and mean & SD from the date when that position was opened.
- Finally, at the end of each day, we accordingly buy or sell quantity no. of pairs.
