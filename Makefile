OS = ${shell uname}
JULIUS = julius-4.3

ifeq ($(OS), Linux)
all:
	cd $(JULIUS) && ./configure --with-mictype=alsa && make
	cp $(JULIUS)/gramtools/mkdfa/mkdfa.pl             tool/mkdfa
	cp $(JULIUS)/gramtools/mkdfa/mkfa-1.44-flex/mkfa  tool/.
	cp $(JULIUS)/gramtools/generate/generate          tool/.
	cp $(JULIUS)/gramtools/dfa_minimize/dfa_minimize  tool/.
	node-gyp rebuild
endif
ifeq ($(OS), Darwin)
all:
	cd $(JULIUS) && ./configure --with-mictype=portaudio && make
	cp $(JULIUS)/gramtools/mkdfa/mkdfa.pl                  tool/mkdfa
	cp $(JULIUS)/gramtools/mkdfa/mkfa-1.44-flex/mkfa.dSYM  tool/mkfa
	cp $(JULIUS)/gramtools/generate/generate.dSYM          tool/generate
	cp $(JULIUS)/gramtools/dfa_minimize/dfa_minimize.dSYM  tool/dfa_minimize
	node-gyp rebuild
endif

clean:
	rm tool/*
	cd $(JULIUS) && make clean && make distclean
	rm -rf node_modules/async node_modules/mecab-async node_modules/kana2voca
	rmdir node_modules

