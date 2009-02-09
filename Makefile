.PHONY: zu3ga clean

all: zu3ga

zu3ga:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean
