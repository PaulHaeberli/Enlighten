all: enlighten.c
	gcc enlighten.c -o enlighten -lm

test: enlighten
	./enlighten Marigold.png MarigoldBright.png 0.6
	./enlighten ThaiCafe.png ThaiCafeBright.png 0.6
	open Marigold.png MarigoldBright.png
	open ThaiCafe.png ThaiCafeBright.png

clean:
	rm -f enlighten MarigoldBright.png ThaiCafeBright.png
