test:
	rm -f eval

run: test eval
	./eval 
	./eval "(0.(0+1)"
	./eval "NON1+0"
	./eval "1.NON0 <=> 0+NON(NON1.1)" 
	./eval "(1=>(NON (1+0).1))"

eval: eval.c
	gcc -Wall  $< -o $@

zip: Makefile cr.tex cr.pdf eval.c
	rm -rf $(NOMS) $(NOMS).zip
	mkdir $(NOMS)
	cp $^ $(NOMS)
	zip -9 -rm $(NOMS).zip $(NOMS)
