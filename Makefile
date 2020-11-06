all:	crazy_coin

sdlmove:	sdlmove.cpp
	g++ sdlmove.cpp -o sdlmove `sdl-config --cflags --libs` 

crazy_coin:	crazy_coin.c
	gcc  crazy_coin.c -o crazy_coin `sdl-config --cflags --libs` -lm

test_snd:	test_snd.c
	gcc  test_snd.c -o test_snd `sdl-config --cflags --libs`

clean:
	rm sdlmove sdlmove2 crazy_coin test_snd *.o
