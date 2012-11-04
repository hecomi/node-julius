OS = ${shell uname}

ifeq ($(OS), Linux)
all:
	cd julius-4.2.2 && ./configure --with-mictype=alsa && make
	npm install
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl             tool/mkdfa
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa  tool/.
	cp julius-4.2.2/gramtools/generate/generate          tool/.
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize  tool/.
endif
ifeq ($(OS), Darwin)
all:
	cd julius-4.2.2 && ./configure --with-mictype=portaudio && make
	npm install
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl                  tool/mkdfa
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa.dSYM  tool/mkfa
	cp julius-4.2.2/gramtools/generate/generate.dSYM          tool/generate
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize.dSYM  tool/dfa_minimize
claen:
	cd julius-4.2.2 && make distclean
	rm -rf node_modules
endif
