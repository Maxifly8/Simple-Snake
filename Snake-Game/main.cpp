#include <vector>
#include <algorithm>
#include <deque>
#include <iostream>
#include <sstream>
#include "SDL.h"
#include "SDL_ttf.h"
#define WIDTH 800
#define HEIGHT 600
#undef main

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Snek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if (!window)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created.");
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created.");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_Event event;
	TTF_Init();
	TTF_Font *font = TTF_OpenFont("E:/VS/snek/Snake-Game/Assets/Pixelletters.ttf", 30); //WIP; must change to your own file path
	if (!font)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Font creation failed: %s", TTF_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return -1;
	}
reset:

	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Simple Snake", textColor);
	if (!textSurface)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Text creation failed: %s", TTF_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return -1;
	}
	SDL_Rect textPos = { 200, 200, textSurface->w * 3, textSurface->h * 3 }; //pos of text

	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if (!textTexture)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create a texture.");
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return -1;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Press any key to continue or ESC to exit...", textColor);
	textPos = { 190, 300, textSurface->w, textSurface->h };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	SDL_RenderPresent(renderer);
	
	bool menu_1 = true;
	while (menu_1)
	{
		if (SDL_PollEvent(&event))
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					TTF_Quit();
					SDL_Quit();
					return 0;

				default:
					menu_1 = false;
					SDL_RenderClear(renderer);
				}
			}
	}
	
	textSurface = TTF_RenderText_Solid(font, "Choose the color of your snake:", textColor);
	textPos = { 85, 100, textSurface->w * 2, textSurface->h * 2};
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Press [P] to choose Purple", textColor);
	textPos = { 110, 180, textSurface->w, textSurface->h };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Press [G] to choose Green", textColor);
	textPos = { 110, 230, textSurface->w, textSurface->h };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Press [W] to White", textColor);
	textPos = { 110, 280, textSurface->w, textSurface->h };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "*Note: the game will start as soon as you press a corresponding button", textColor);
	textPos = { 40, 550, textSurface->w, textSurface->h };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	SDL_RenderPresent(renderer);
	
	enum directions_and_colors { down, left, right, up , white, purple, green};
	short snake_color = white;
	bool menu_2 = true;
	while (menu_2)
	{
		if (SDL_PollEvent(&event))
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					TTF_Quit();
					SDL_Quit();
					return 0;

				case SDLK_w:
					menu_2 = false;
					snake_color = white;
					break;

				case SDLK_p:
					menu_2 = false;
					snake_color = purple;
					break;

				case SDLK_g:
					menu_2 = false;
					snake_color = green;
					break;
				}
			}
	}

	/*
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
	TTF_CloseFont(font);
	*/

	bool quit = false;
	
	short direction = up;
	short previous_dir = down;
	int score = 0;

	unsigned int snake_length = 1;
	SDL_Rect head{ WIDTH >> 1, HEIGHT >> 1, 10, 10 }; //snake head position
	std::deque<SDL_Rect> rq; //rectangle double ended queue for snakes body
	std::vector<SDL_Rect> fruits; //rects for fruits
	for (int i = 0; i < 10; ++i) //create fruits on the map
	{
		SDL_Rect fruit{ rand() % (WIDTH/10) * 10, rand() % (HEIGHT/10) * 10, 10, 10 };
		fruits.push_back(fruit);
	}


	while (!quit) //main loop
	{
		if (SDL_PollEvent(&event)) //check input
		{
			if (event.type == SDL_QUIT)
			{
				SDL_Quit();
				exit(0);
			}

			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;

				case SDLK_w:
					if(previous_dir == down)
						break;
					direction = up;
					break;

				case SDLK_d:
					if (previous_dir == left)
						break;
					direction = right;
					break;

				case SDLK_s:
					if (previous_dir == up)
						break;
					direction = down;
					break;

				case SDLK_a:
					if (previous_dir == right)
						break;
					direction = left;
					break;
				}
				previous_dir = direction;
			}
			
		}

		switch (direction)
		{
		case up:
			head.y -= 10;
			break;

		case down:
			head.y += 10;
			break;

		case left:
			head.x -= 10;
			break;

		case right:
			head.x += 10;
			break;

		}

		std::for_each(fruits.begin(), fruits.end(), [&](auto& fruit) //collision detection with fruit
			{
				if (head.x == fruit.x && head.y == fruit.y)
				{
					snake_length += 5;
					fruit.x = rand() % (WIDTH / 10) * 10;
					fruit.y = rand() % (HEIGHT / 10) * 10;
					++score;
				}
			});

		std::for_each(rq.begin(), rq.end(), [&](auto& body) //collision detection with itself
			{
				if (head.x == body.x && head.y == body.y)
					quit = true;
			});

		if (head.x == 0 || head.y == 0 || head.x == WIDTH || head.y == HEIGHT) //collision detection with window border
			quit = true;

		rq.push_front(head);
		for( ; rq.size() > snake_length; )
			rq.pop_back();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if (snake_color == purple)
		{
			SDL_SetRenderDrawColor(renderer, 255, 90, 255, 255); //color for head
			SDL_RenderFillRect(renderer, &rq[0]);
			SDL_SetRenderDrawColor(renderer, 255, 190, 255, 255); //color for body
		}

		else if (snake_color == white)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(renderer, &rq[0]);
			SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
		}
		
		else if (snake_color == green)
		{
			SDL_SetRenderDrawColor(renderer, 90, 255, 90, 255);
			SDL_RenderFillRect(renderer, &rq[0]);
			SDL_SetRenderDrawColor(renderer, 190, 255, 190, 255);
		}
		
		std::for_each(rq.begin() + 1, rq.end(), [&](auto &body) //draw body
			{
				SDL_RenderFillRect(renderer, &body);
			});
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		std::for_each(fruits.begin(), fruits.end(), [&](auto &fruit)
			{
				SDL_RenderFillRect(renderer, &fruit);
			});




		SDL_RenderPresent(renderer);
		SDL_Delay(25);
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	textSurface = TTF_RenderText_Solid(font, "GAME OVER!", textColor);
	textPos = { 145, 90, textSurface->w * 4, textSurface->h * 4};
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Your Score:", textColor);
	textPos = { 190, 250, textSurface->w * 3, textSurface->h * 3 };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	std::stringstream strm;
	strm << score;
	textSurface = TTF_RenderText_Solid(font, strm.str().c_str(), textColor); //display score
	textPos = { 560, 250, textSurface->w * 3, textSurface->h * 3 };
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	textSurface = TTF_RenderText_Solid(font, "Press ESC to exit or [R] to restart...", textColor);
	textPos = { 30, 530, textSurface->w * 2, textSurface->h * 2};
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

	SDL_RenderPresent(renderer);

	bool menu_3 = true;
	while (menu_3)
	{
		if (SDL_PollEvent(&event))
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					menu_3 = false;
					break;

				case SDLK_r:
					goto reset;
				}
			}
	}

	TTF_Quit();
	SDL_Quit();
	return 0;
}