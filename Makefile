All:
	cd UTILS/SOURCE ; make
	cd LIBERTAD/SOURCE ; make
	cd LIBERTAD/EXAMPLES ; make
	cd MINILOG/SOURCE ; make
	cd MINILOG/EXAMPLES ; make
	cd EXAMPLES ; make

Doc:
	cd LIBERTAD/SOURCE ; make Doc
	cd MINILOG/SOURCE ; make Doc

Tar:
	make clean
	make Doc
	tar -czv --exclude='CVS' -f ../NTS.tgz ../NTS
	mv ../NTS.tgz .

clean:
	rm -f NTS.tgz
	cd UTILS/SOURCE ; make clean
	cd LIBERTAD/SOURCE ; make clean
	cd LIBERTAD/EXAMPLES ; make clean
	cd MINILOG/SOURCE ; make clean
	cd MINILOG/EXAMPLES ; make clean
	cd EXAMPLES ; make clean
