SOURCES = src/game.c src/tileset.c src/sound.c src/multi.c src/room_manager.c src/room_templates.c src/player.c src/item_manager.c src/map_view.c src/transition.c src/quiz_mode.c src/game_state.c src/globals.c

all: build/game.gb

build/game.gb: $(SOURCES)
	C:/gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -msm83:gb -o build/game.gb $(SOURCES)

clean:
	cmd /c if exist "build\*.*" del /Q "build\*.*"