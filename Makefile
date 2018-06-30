
all:
	cd DataIO && "$(MAKE)"
	cd sercode  && "$(MAKE)"
	cd wvformview  && "$(MAKE)"
	cd mcrbee10  && "$(MAKE)"

clean:
	cd DataIO && "$(MAKE)" clean
	cd sercode  && "$(MAKE)" clean
	cd wvformview  && "$(MAKE)" clean
	cd mcrbee10  && "$(MAKE)" clean
