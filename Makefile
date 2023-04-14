.PHONY: clean install libs monolith

install:
	$(MAKE) -f module.mk $(MAKECMDGOALS)
	$(MAKE) -f executable.mk $(MAKECMDGOALS)

libs:
	cd lib/rocket; $(MAKE) lib/librocket.a lib/librocket-player.a

monolith:
	$(MAKE) -f monolithic.mk

clean:
	rm -rf debug release
