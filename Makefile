.PHONY: clean install libs monolithic

clean install:
	$(MAKE) -f executable.mk $(MAKECMDGOALS)
	$(MAKE) -f module.mk $(MAKECMDGOALS)

libs:
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a

monolithic:
	make -f monolithic.mk
