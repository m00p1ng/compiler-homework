CC=gcc
TARGET=expreval

obj:
	$(CC) $(TARGET).c -o $(TARGET)

good: obj
	./expreval good1.txt
	./expreval good2.txt
	./expreval good3.txt
	./expreval good4.txt

bad: obj
	./expreval bad${case}.txt

clean:
	rm ./expreval
