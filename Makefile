OS = ${shell uname}

ifeq ($(OS), Linux)
all:
	cd julius-4.2.2 && ./configure --with-mictype=alsa && make
	npm install
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl node_modules/JuliusGrammar/tool/.
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa node_modules/JuliusGrammar/tool/.
	cp julius-4.2.2/gramtools/generate/generate node_modules/JuliusGrammar/tool/.
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize node_modules/JuliusGrammar/tool/.
endif
ifeq ($(OS), Darwin)
all:
	cd julius-4.2.2 && ./configure --with-mictype=portaudio && make
	npm install
	cp julius-4.2.2/gramtools/mkdfa/mkdfa.pl node_modules/JuliusGrammar/tool/.
	cp julius-4.2.2/gramtools/mkdfa/mkfa-1.44-flex/mkfa.dSYM node_modules/JuliusGrammar/tool/mkfa
	cp julius-4.2.2/gramtools/generate/generate.dSYM node_modules/JuliusGrammar/tool/generate
	cp julius-4.2.2/gramtools/dfa_minimize/dfa_minimize.dSYM node_modules/JuliusGrammar/tool/dfa_minimize
endif
