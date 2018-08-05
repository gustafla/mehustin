.PHONY: libs

libs:
	cd lib/rocket; make lib/librocket.a lib/librocket-player.a

.DEFAULT:
	make -f executable.mk $(MAKECMDGOALS)
	make -f module.mk $(MAKECMDGOALS)

