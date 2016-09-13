OS      = ${shell uname}
JULIUS  = julius-4.4.2
TOOLDIR = tool

ifeq ($(OS), Linux)
FLAGS_AFTER = --with-mictype=alsa --enable-shared
FLAGS_BEFORE = CFLAGS=-fPIC CXXFLAGS=-fPIC
else
ifeq ($(OS), Darwin)
FLAGS_AFTER = --with-mictype=portaudio
FLAGS_BEFORE =
else
FLAGS =
FLAGS_BEFORE =
endif
endif

all:
	mkdir -p $(TOOLDIR)
	cd $(JULIUS) && $(FLAGS_BEFORE) ./configure $(FLAGS_AFTER) && make
	cp $(JULIUS)/gramtools/mkdfa/mkdfa.pl             $(TOOLDIR)/mkdfa
	cp $(JULIUS)/gramtools/mkdfa/mkfa-1.44-flex/mkfa  $(TOOLDIR)/.
	cp $(JULIUS)/gramtools/generate/generate          $(TOOLDIR)/.
	cp $(JULIUS)/gramtools/dfa_minimize/dfa_minimize  $(TOOLDIR)/.
	node-gyp rebuild

clean:
	rm -rf $(TOOLDIR)
	cd $(JULIUS) && make clean && make distclean
	rm -rf node_modules/async node_modules/mecab-async node_modules/kana2voca
	rmdir node_modules

