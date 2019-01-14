.PHONY: build clean install libs monolithic

build clean install:
	make -f executable.mk $(MAKECMDGOALS)
	make -f module.mk $(MAKECMDGOALS)

libs:
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a

monolithic:
	make -f monolithic.mk release
