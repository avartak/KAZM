LDFLAGS  = -lreflex
LEXER   := $(shell which reflex)
SOURCES := $(wildcard src/*.cc)
OBJECTS := $(patsubst %.cc,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cc,%.d,$(SOURCES))

.PHONY: all clean

all: kazm

clean:
	$(RM) $(OBJECTS) $(DEPENDS)

src/Scanner.cc: src/lexer.l
	$(LEXER) --lexer=Scanner --namespace=kazm --noline --lex=scan −−token-type=kazm::Token --header-file=include/Scanner.h -o src/Scanner.cc src/lexer.l

%.o: %.cc Makefile
	clang++ -O3 -c -std=c++14 $(CXXFLAGS) -MMD -MP -I$(PWD)/include -o $@ $<

kazm: $(OBJECTS)
	clang++ -O3 -o $@ $(OBJECTS) $(LDFLAGS)

-include $(DEPENDS)
