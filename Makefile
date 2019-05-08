.PHONY: clean install libs monolithic

clean install:
	$(MAKE) -f module.mk $(MAKECMDGOALS)
	$(MAKE) -f executable.mk $(MAKECMDGOALS)

libs:
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a

monolithic:
	make -f monolithic.mk
