#include <math.h>
#include <stdlib.h>
#include <time.h>
#ifndef PI
#define PI 3.14159265359
#endif

#include "SDL.h"
#include "sdlmove2.h"

#define SCREEN_WIDTH  992
#define SCREEN_HEIGHT 640
#define SPRITE_SIZE    32

int hit_line(SDL_Rect *line_posns, size_t n, SDL_Rect *curpos, int offx, 
	      int offy, int sprite_size) {
  size_t i;
  int x = curpos->x + offx;
  int y = curpos->y + offy;
  for (i=0; i<n; i++)
    if (line_posns[i].x < x+sprite_size && 
	line_posns[i].x+line_posns[i].w > x &&
	line_posns[i].y < y+sprite_size &&
	line_posns[i].y+line_posns[i].h > y) {
      printf("line %ld x=%d y=%d w=%d h=%d, sprite x=%d, y=%d\n",
	     i+1, line_posns[i].x,  line_posns[i].y, line_posns[i].w,
	     line_posns[i].h, x, y);
      return(i+1);
    }
  return(0);
}


int main(int argc, char* argv[])
{
	SDL_Surface *screen, *temp, *sprite, *grass, *castle, *something,
	  *numbers[10], *coin, *gameover_bmp;
	SDL_Rect rcSprite, rcSprite_last, rcGrass, rcCastle, rcSomething, 
	  *line_posns, line_pos, timer_pos_ones, timer_pos_tens, coin_pos,
	  gameover_pos, coin_count_pos, coin_count_line;
	SDL_Event event;
	Uint8 *keystate;
	Uint32 red, blue,color, black;
	int colorkey, gameover, white_colorkey;
	int i, x, y, offx, offy, line, last_line=0;
	double phi;
	long dx, dy;
	Uint32 now, wh_start, start_time, coin_time;
	int flash = 0;
	int ones, tens, seconds, time_left;
	char fname[6] = "0.bmp";
	int coins_found = 0;
	float coin_offx = 0.1*(1+random() % 10);
	float coin_offy = 0.1+(1+random() % 10);
	int quad_x, quad_y; // Quadrant the coin is currently in
	double coin_x, coin_y;

	/* initialize video system */
	SDL_Init(SDL_INIT_VIDEO);

	/* set the title bar */
	SDL_WM_SetCaption("Watch out for the well!", "SDL Move");

	/* create window */
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	blue = SDL_MapRGB(screen->format, 0, 0, 255);
	red = SDL_MapRGB(screen->format, 255, 0, 0);
	black = SDL_MapRGB(screen->format, 0, 0, 0);


	/* Blank line at top */
	coin_count_line.x = 0;
	coin_count_line.y = 0;
	coin_count_line.h = SPRITE_SIZE;
	coin_count_line.w = SCREEN_WIDTH;
	
	/* Setup lines */
	
	line_pos.x = SCREEN_WIDTH/2;
	line_pos.y = 0;
	line_pos.w = 2;
	line_pos.h = SCREEN_HEIGHT/4;
	line_posns = (SDL_Rect *) malloc(7*sizeof(SDL_Rect));
	line_posns[0] = line_pos;
	line_posns[1].x = SCREEN_WIDTH/8;
	line_posns[1].y = SCREEN_HEIGHT/2;
	line_posns[1].h = 2;
	line_posns[1].w = SCREEN_WIDTH/4;
	line_posns[2].x = 5*SCREEN_WIDTH/8;
	line_posns[2].y = SCREEN_HEIGHT/2;	
	line_posns[2].h = 2;
	line_posns[2].w = SCREEN_WIDTH/4;
	line_posns[3].x = 3*SCREEN_WIDTH/8;
	line_posns[3].y = 3*SCREEN_HEIGHT/4;
	line_posns[3].h = SCREEN_HEIGHT/4;
	line_posns[3].w = 2;
	line_posns[4].x = 5*SCREEN_WIDTH/8;
	line_posns[4].y = 3*SCREEN_HEIGHT/4;
	line_posns[4].h = SCREEN_HEIGHT/4;
	line_posns[4].w = 2;
	line_posns[5].x = 7*SCREEN_WIDTH/16;
	line_posns[5].y = 15*SCREEN_HEIGHT/32;
	line_posns[5].h = SCREEN_HEIGHT/16;
	line_posns[5].w = 2;
	line_posns[6].x = 9*SCREEN_WIDTH/16;
	line_posns[6].y = 15*SCREEN_HEIGHT/32;
	line_posns[6].h = SCREEN_HEIGHT/16;
	line_posns[6].w = 2;

	/* load sprite */
	temp   = SDL_LoadBMP("sprite.bmp");
	sprite = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	temp = SDL_LoadBMP("game_over.bmp");
	white_colorkey = SDL_MapRGB(screen->format, 255, 255, 255);
	gameover_bmp = SDL_DisplayFormat(temp);
	SDL_SetColorKey(gameover_bmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, white_colorkey);

	SDL_FreeSurface(temp);


	/* Load coin */
	temp = SDL_LoadBMP("coin_purple.bmp");
	coin = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	coin_pos.x = random() % (SCREEN_WIDTH-SPRITE_SIZE-1);
	coin_pos.y = (random() % (SCREEN_HEIGHT-2*SPRITE_SIZE-1)) + 
	  SPRITE_SIZE;
	coin_x = coin_pos.x;
	coin_y = coin_pos.y;


	/* setup sprite colorkey and turn on RLE */
	colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

	/* coin color key (white) */
	SDL_SetColorKey(coin, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
 

	/* load grass */
	temp  = SDL_LoadBMP("grass.bmp");
	grass = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	/* Load lame castle */
	temp = SDL_LoadBMP("lame_castle.bmp");
	castle = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_SetColorKey(castle, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

	temp = SDL_LoadBMP("something.bmp");
	something = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	/* Load numbers */
	for (i=0; i<10; i++) {
	  printf("%s ", fname);
	  temp = SDL_LoadBMP(fname);
	  numbers[i] = SDL_DisplayFormat(temp);
	  SDL_FreeSurface(temp);
	  fname[0] += 1;
	}
	printf("\n");


	gameover = 0;
	gameover_pos.x = SCREEN_WIDTH/2-40;
	gameover_pos.y = 0.3*SCREEN_HEIGHT;

	/* Set timer position */
	timer_pos_ones.x = SCREEN_WIDTH - SPRITE_SIZE;
	timer_pos_ones.y = 0;
	timer_pos_tens.x = SCREEN_WIDTH - 1.6*SPRITE_SIZE;
	timer_pos_tens.y = 0;


	while (!gameover) {
	  
	  coins_found = 0;
	  /* Reset status line */
	  SDL_FillRect(screen, &coin_count_line, black);

	  /* set sprite position */
	  rcSprite.x = 0;
	  rcSprite.y = SPRITE_SIZE;
	  rcSprite_last.x = 0;
	  rcSprite_last.y = SPRITE_SIZE;
	  
	  /* draw the grass */
	  for (x = 0; x < SCREEN_WIDTH/SPRITE_SIZE; x++) {
	    for (y = 0; y < (SCREEN_HEIGHT-SPRITE_SIZE)/SPRITE_SIZE; y++) {
	      rcGrass.x = x * SPRITE_SIZE;
	      rcGrass.y = (y+1) * SPRITE_SIZE;
	      SDL_BlitSurface(grass, NULL, screen, &rcGrass);
	    }
	  }
	  
	  start_time = SDL_GetTicks();
	  
	  /* message pump */
	  while (!gameover)
	    {
	      /* look for an event */
	      if (SDL_PollEvent(&event)) {
		/* an event was found */
		switch (event.type) {
				/* close button clicked */
		case SDL_QUIT:
		  gameover = 2;
		  break;
		  
				/* handle the keyboard */
		case SDL_KEYDOWN:
		  switch (event.key.keysym.sym) {
		  case SDLK_ESCAPE:
		  case SDLK_q:
		    gameover = 2;
		    break;
		  }
		  break;
		}
	      }
	      
	      /* handle sprite movement */
	      keystate = SDL_GetKeyState(NULL);
	      offx = 0;
	      offy = 0;
	      
	      if (keystate[SDLK_LEFT] ) {
		//rcSprite.x -= 2;
		offx = -2;
	      }
	      if (keystate[SDLK_RIGHT] ) {
		//rcSprite.x += 2;
		offx = 2;
	      }
	      if (keystate[SDLK_UP] ) {
		//rcSprite.y -= 2;
		offy = -2;
	      }
	      if (keystate[SDLK_DOWN] ) {
		//rcSprite.y += 2;
		offy = 2;
	      }
	      
	      if (hit_line(line_posns, 7, &rcSprite, offx, offy, 
			   SPRITE_SIZE)) {
		offx = 0;
		offy = 0;
	      }
	      rcSprite.x += offx;
	      rcSprite.y += offy;
	      
	      /* collide with edges of screen */
	      if ( rcSprite.x < 0 ) {
		rcSprite.x = 0;
	      }
	      else if ( rcSprite.x > SCREEN_WIDTH-SPRITE_SIZE ) {
		rcSprite.x = SCREEN_WIDTH-SPRITE_SIZE;
	      }
	      if ( rcSprite.y < SPRITE_SIZE) {
		rcSprite.y = SPRITE_SIZE;
	      }
	      else if ( rcSprite.y > SCREEN_HEIGHT-SPRITE_SIZE ) {
		rcSprite.y = SCREEN_HEIGHT-SPRITE_SIZE;
	      }
	      
	      if (rcSprite_last.x != rcSprite.x || 
		  rcSprite_last.y != rcSprite.y) {
		SDL_BlitSurface(grass, NULL, screen, &rcSprite_last);
		rcSprite_last.x = rcSprite.x;
		rcSprite_last.y = rcSprite.y;
	      }
	      
	      /* draw the sprite */
	      SDL_BlitSurface(sprite, NULL, screen, &rcSprite);
	      
	      coin_offx += ((random() % 10) - 5) / 20.;
	      coin_offy += ((random() % 10) - 5)/ 20.;
	      if (coin_offx > 2.)
		coin_offx = 2.;
	      if (coin_offx < -2.)
		coin_offx = -2.;
	      if (coin_offy > 2.)
		coin_offy = 2.;
	      if (coin_offy < -2.)
		coin_offy = -2.;

	      /* See if we hit the coin */
	      if (rcSprite.x >= coin_pos.x-SPRITE_SIZE/2 && 
		  rcSprite.x < coin_pos.x+SPRITE_SIZE/2 &&
		  rcSprite.y >= coin_pos.y-SPRITE_SIZE/2 &&
		    rcSprite.y < coin_pos.y+SPRITE_SIZE/2) {
		  coins_found += 1;
		  // Move coin to a new random pos.
		  SDL_BlitSurface(grass, NULL, screen, &coin_pos);
		  line = 1;
		  while (line) { // loop to avoid appearing on a line
		    coin_pos.x = random() % (SCREEN_WIDTH-SPRITE_SIZE-1);
		    coin_pos.y = (random() % (SCREEN_HEIGHT-2*SPRITE_SIZE-1)) +
		      SPRITE_SIZE;
		    line = hit_line(line_posns, 7, &coin_pos, 0, 0, 
				    SPRITE_SIZE);
		  }
		  coin_x = coin_pos.x;
		  coin_y = coin_pos.y;
		}

		/* Draw the lame castle */
		rcCastle.x = SCREEN_WIDTH - SPRITE_SIZE;
		rcCastle.y = SCREEN_HEIGHT - SPRITE_SIZE;
		SDL_BlitSurface(castle, NULL, screen, &rcCastle);

		rcSomething.x = (SCREEN_WIDTH-SPRITE_SIZE)/2;
		rcSomething.y = (SCREEN_HEIGHT-SPRITE_SIZE)/2;
		SDL_BlitSurface(something, NULL, screen, &rcSomething);

		/* Draw line */
		// SDL_FillRect(screen, &line_pos, 0);
		for (i=0; i<7; i++)
		  SDL_FillRect(screen, &line_posns[i], 0);

		/* Draw circle */
		
		if (flash) {
		  color = red;
		  if (SDL_GetTicks() - wh_start > 100) {
		    printf("Setting flash to 0\n");
		    flash = 0;
		  }
		} else
		  color = blue;
		dx = (SCREEN_WIDTH/2 - SPRITE_SIZE/2 - rcSprite.x);
		dy = (SCREEN_HEIGHT/2 - SPRITE_SIZE/2 - rcSprite.y);
		if (dx*dx + dy*dy < 625) {
		  flash = 1;
		  wh_start = SDL_GetTicks();
		  #ifdef WH_RANDOM
		  rcSprite.x = random() % (SCREEN_WIDTH-SPRITE_SIZE-1);
		  rcSprite.y = (random() % (SCREEN_HEIGHT-2*SPRITE_SIZE-1))
		    + SPRITE_SIZE;
		  #else
		  // Put player in same quandrant as coin
		  quad_x = coin_pos.x/(SCREEN_WIDTH/2);
		  quad_y = coin_pos.y/(SCREEN_HEIGHT/2);
		  rcSprite.x = random() % (SCREEN_WIDTH/2-SPRITE_SIZE-1) + 
		    quad_x*SCREEN_WIDTH/2;
		  rcSprite.y = random() % (SCREEN_HEIGHT/2-SPRITE_SIZE-1) + 
		    quad_y*SCREEN_HEIGHT/2;
		  #endif
		  
		  while (hit_line(line_posns, 7, &rcSprite, offx, offy, 
				  SPRITE_SIZE)) {
		    #ifdef WH_RANDOM
		    rcSprite.x = random() % (SCREEN_WIDTH-SPRITE_SIZE-1);
		    rcSprite.y = (random() % (SCREEN_HEIGHT-2*SPRITE_SIZE-1))
		      + SPRITE_SIZE;
		    #else
		    rcSprite.x = random() % (SCREEN_WIDTH/2-SPRITE_SIZE-1) + 
		      quad_x*SCREEN_WIDTH/2;
		    rcSprite.y = random() % (SCREEN_HEIGHT/2-SPRITE_SIZE-1) + 
		      quad_y*SCREEN_HEIGHT/2;
		    #endif
		  }
		    
		}

		for (phi=0; phi<360; phi++) {
		  x = SCREEN_WIDTH/2 + 25*cos(phi*PI/180.);
		  y = SCREEN_HEIGHT/2 + 25*sin(phi*PI/180.);
		  line_pos.x = x;
		  line_pos.y = y;
		  line_pos.w = 4;
		  line_pos.h = 4;
		  SDL_FillRect(screen, &line_pos, color);
		}
		    
		/* Draw timer */
		seconds = (SDL_GetTicks() - start_time)/1000;
		time_left = 60 - seconds;
		if (time_left < 1)
		  gameover = 1;
		ones = time_left % 10;
		tens = time_left / 10;
		SDL_BlitSurface(numbers[tens], NULL, screen, &timer_pos_tens);
		SDL_BlitSurface(numbers[ones], NULL, screen, &timer_pos_ones);

		/* Draw coin */
		/*
		if ((SDL_GetTicks() - coin_time)/1000 > 10) {
		  SDL_BlitSurface(grass, NULL, screen, &coin_pos);
		  coin_pos.x = random() % (SCREEN_WIDTH-SPRITE_SIZE-1);
		  coin_pos.y = random() % (SCREEN_HEIGHT-SPRITE_SIZE-1);
		  coin_time = SDL_GetTicks();
		}
		*/
		line = hit_line(line_posns, 7, &coin_pos, 
				coin_offx, coin_offy, 
				SPRITE_SIZE);
		
		if (line==0) {
		  if (coin_pos.x + coin_offx < 0 || 
		      coin_pos.x + coin_offx >= SCREEN_WIDTH-SPRITE_SIZE)
		    coin_offx = -coin_offx;
		  if (coin_pos.y + coin_offy < SPRITE_SIZE || coin_pos.y + 
		      coin_offy >= SCREEN_HEIGHT-SPRITE_SIZE)
		    coin_offy = -coin_offy;
		  SDL_BlitSurface(grass, NULL, screen, &coin_pos);
		  coin_x += coin_offx;
		  coin_y += coin_offy;
		  coin_pos.x = (int) coin_x;
		  coin_pos.y = (int) coin_y;
		} else {
		  if (line == last_line) {
		    coin_offx = -coin_offx;
		    coin_offy = -coin_offy;
		  } else {
		    if (line_posns[line-1].w == 2) {
		      printf("coin hit line %d, reversing x\n", line);
		      coin_offx = -coin_offx;
		    } else
		      coin_offy = -coin_offy;
		  }
		}
		last_line = line;
		SDL_BlitSurface(coin, NULL, screen, &coin_pos);

		// Draw found coins
		for (i=0; i<coins_found; i++) {
		  coin_count_pos.x = i*SPRITE_SIZE;
		  coin_count_pos.y = 0;
		  SDL_BlitSurface(coin, NULL, screen, &coin_count_pos);
		}
		
		/* update the screen */
		SDL_UpdateRect(screen,0,0,0,0);
	}

	gameover = 0;
	SDL_BlitSurface(gameover_bmp, NULL, screen, &gameover_pos);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	while (gameover != -1 && gameover!=1) {
	  /* look for an event */
	  if (SDL_PollEvent(&event)) {
	    /* an event was found */
	    switch (event.type) {
				/* close button clicked */
	    case SDL_QUIT:
	      gameover = 1;
	      break;
	    case SDL_MOUSEBUTTONDOWN:
	      gameover = -1;
	      break;
	    case SDL_KEYDOWN:
	      gameover = -1;
	      break;
	    }
	    
	  }
	}
	if (gameover < 0)
	  gameover = 0;
	
	}
	/* clean up */
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(grass);
	SDL_Quit();

	return 0;
}

