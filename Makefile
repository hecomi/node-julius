OS = ${shell uname}

ifeq ($(OS), Linux)
all:
	cp -r node_modules/pre/kana2voca node_modules/.
	cd julius-4.2.2 && ./configure --with-mictype=alsa && make
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl             tool/mkdfa
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa  tool/.
	cp julius-4.2.2/gramtools/generate/generate          tool/.
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize  tool/.
	node-gyp rebuild
endif
ifeq ($(OS), Darwin)
all:
	cp -r node_modules/pre/kana2voca node_modules/.
	cd julius-4.2.2 && ./configure --with-mictype=portaudio && make
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl                  tool/mkdfa
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa.dSYM  tool/mkfa
	cp julius-4.2.2/gramtools/generate/generate.dSYM          tool/generate
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize.dSYM  tool/dfa_minimize
	node-gyp rebuild
endif

clean:
	cd julius-4.2.2 && make clean && make distclean
	rm -rf node_modules/async node_modules/mecab-async node_modules/kana2voca
