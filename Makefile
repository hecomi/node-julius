OS      = ${shell uname}
JULIUS  = julius-4.4.2
TOOLDIR = tool

ifeq ($(OS), Linux)
FLAGS = --with-mictype=alsa
else
ifeq ($(OS), Darwin)
FLAGS = --with-mictype=portaudio
else
FLAGS =
endif
endif

all:
	mkdir -p $(TOOLDIR)
	cd $(JULIUS) && ./configure $(FLAGS) && make
	cp $(JULIUS)/gram$(TOOLDIR)s/mkdfa/mkdfa.pl             $(TOOLDIR)/mkdfa
	cp $(JULIUS)/gram$(TOOLDIR)s/mkdfa/mkfa-1.44-flex/mkfa  $(TOOLDIR)/.
	cp $(JULIUS)/gram$(TOOLDIR)s/generate/generate          $(TOOLDIR)/.
	cp $(JULIUS)/gram$(TOOLDIR)s/dfa_minimize/dfa_minimize  $(TOOLDIR)/.
	node-gyp rebuild

clean:
	rm -rf $(TOOLDIR)
	cd $(JULIUS) && make clean && make distclean
	rm -rf node_modules/async node_modules/mecab-async node_modules/kana2voca
	rmdir node_modules

