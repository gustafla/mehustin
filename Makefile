.PHONY: libs debug release clean install

libs:
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a

debug release clean install:
	make -f executable.mk $(MAKECMDGOALS)
	make -f module.mk $(MAKECMDGOALS)

