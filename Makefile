OS      = ${shell uname}
JULIUS  = julius-4.3
TOOLDIR = tool

ifeq ($(OS), Linux)
all:
	mkdir $(TOOLDIR)
	cd $(JULIUS) && ./configure --with-mictype=alsa && make
	cp $(JULIUS)/gram$(TOOLDIR)s/mkdfa/mkdfa.pl             $(TOOLDIR)/mkdfa
	cp $(JULIUS)/gram$(TOOLDIR)s/mkdfa/mkfa-1.44-flex/mkfa  $(TOOLDIR)/.
	cp $(JULIUS)/gram$(TOOLDIR)s/generate/generate          $(TOOLDIR)/.
	cp $(JULIUS)/gram$(TOOLDIR)s/dfa_minimize/dfa_minimize  $(TOOLDIR)/.
	node-gyp rebuild
endif
ifeq ($(OS), Darwin)
all:
	mkdir $(TOOLDIR)
	cd $(JULIUS) && ./configure --with-mictype=portaudio && make
	cp $(JULIUS)/gramtools/mkdfa/mkdfa.pl                  $(TOOLDIR)/mkdfa
	cp $(JULIUS)/gramtools/mkdfa/mkfa-1.44-flex/mkfa.dSYM  $(TOOLDIR)/mkfa
	cp $(JULIUS)/gramtools/generate/generate.dSYM          $(TOOLDIR)/generate
	cp $(JULIUS)/gramtools/dfa_minimize/dfa_minimize.dSYM  $(TOOLDIR)/dfa_minimize
	node-gyp rebuild
endif

clean:
	rm -rf $(TOOLDIR)
	cd $(JULIUS) && make clean && make distclean
	rm -rf node_modules/async node_modules/mecab-async node_modules/kana2voca
	rmdir node_modules

