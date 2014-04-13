all:
	make -C lib/ 
	make -C server/
	make -C client/
clean:
	make clean -C lib/	
	make clean -C server/
	make clean -C client/
