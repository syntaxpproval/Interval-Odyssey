SOURCES = src/game.c src/game_state.c src/globals.c src/item_manager.c src/map_view.c src/multi.c \
         src/player.c src/quiz_mode.c src/room_manager.c src/room_templates.c src/sound.c \
         src/text.c src/tileset.c src/transition.c src/utils.c src/sequencer.c src/settings.c \
         src/test_tiles.c

all: build/game.gb

build/game.gb: $(SOURCES)
	C:/gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -Wl-yt0x1b -Wl-ya4 -msm83:gb -o build/game.gb $(SOURCES)

clean:
	cmd /c if exist "build\*.*" del /Q "build\*.*"