ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Set PVSNESLIB_HOME to your PVSnesLib folder")
endif

export ROMNAME := starling_hill
export ROMTITLE := STRANGE THINGS STARLING HILL
export ROMVERSION := 0
export SRAMSIZE := 0

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

all: $(ROMNAME).sfc

clean: cleanBuildRes cleanRom
