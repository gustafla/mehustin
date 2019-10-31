.PHONY: clean install libs monolithic

clean install:
	$(MAKE) -f module.mk $(MAKECMDGOALS)
	$(MAKE) -f executable.mk $(MAKECMDGOALS)

libs:
	cd lib/rocket; $(MAKE) lib/librocket.a lib/librocket-player.a

monolithic:
	$(MAKE) -f monolithic.mk
