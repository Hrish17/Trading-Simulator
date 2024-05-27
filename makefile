.PHONY: all

all: $(strategy)

BASIC: basic
DMA: dma
DMA++ : dma++
MACD : macd
RSI : rsi
ADX : adx
LINEAR_REGRESSION : lr
BEST_OF_ALL : boa
PAIRS : pairs


basic:
	python3 main.py "1" $(symbol) ${n} $(start_date) $(end_date)
	g++ basic.cpp -o basic
	./basic $(n) $(x)
	rm basic

dma:
	python3 main.py "1" $(symbol) ${n} $(start_date) $(end_date)
	g++ dma.cpp -o dma
	./dma $(n) $(x) $(p)
	rm dma

dma++:
	python3 main.py "1" $(symbol) ${n} $(start_date) $(end_date)
	g++ dma++.cpp -o dma++
	./dma++ $(n) $(x) $(p) ${max_hold_days} ${c1} ${c2}
	rm dma++

macd:
	python3 main.py "1" $(symbol) "0" $(start_date) $(end_date)
	g++ macd.cpp -o macd
	./macd $(x)
	rm macd

rsi:
	python3 main.py "1" $(symbol) ${n} $(start_date) $(end_date)
	g++ rsi.cpp -o rsi
	./rsi $(n) $(x) $(oversold_threshold) $(overbought_threshold)
	rm rsi

adx:
	python3 main.py "2" $(symbol) "1" $(start_date) $(end_date)
	g++ adx.cpp -o adx
	./adx $(n) $(x) $(adx_threshold)
	rm adx

boa:
	python3 main.py "5" $(symbol) $(start_date) $(end_date)
	g++ best_of_all.cpp -o best_of_all
	./best_of_all $(symbol) $(start_date) $(end_date)
	rm best_of_all

lr:
	python3 main.py "3" $(symbol) ${train_start_date} ${train_end_date} ${start_date} ${end_date}
	g++ linear_regression.cpp -o linear_regression
	./linear_regression $(x) $(p)
	rm linear_regression

pairs:
	if [ "$(stop_loss_threshold)" != "" ]; then \
		python3 main.py "4" $(symbol1) $(symbol2) ${n} $(start_date) $(end_date); \
		g++ pairs2.cpp -o pairs2; \
		./pairs2 $(n) $(x) $(threshold) $(stop_loss_threshold); \
		rm pairs2; \
	else \
		python3 main.py "4" $(symbol1) $(symbol2) ${n} $(start_date) $(end_date); \
		g++ pairs1.cpp -o pairs1; \
		./pairs1 $(n) $(x) $(threshold); \
		rm pairs1; \
	fi