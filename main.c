#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keycode.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} sdl_t;

typedef struct {
  uint32_t window_width;
  uint32_t window_height;
  uint32_t scale_factor;
} config_t;

typedef enum { QUIT, RUNNING, PAUSED } emulator_state_t;

typedef struct {
  emulator_state_t state;
  uint8_t ram[4096];
  bool display[64 * 32]; // emulate original CHIP8 resolution pixels
  uint16_t stack[12];
  uint8_t V[16];       // Data registers V0-VF
  uint8_t I;           // Index Register
  uint16_t PC;         // Program Counter
  uint8_t delay_timer; // Decrements at 60Hz when > 0
  uint8_t sound_timer; // Decrements at 60Hz and plays tone when > 0
  bool keypad[16];     // Hexadecimal Keypad 0x0 - 0xF
  char *rom_name;      // currently running ROM
} chip8_t;

bool init_sdl(sdl_t *sdl, const config_t config) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }

  sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 config.window_width * config.scale_factor,
                                 config.window_height * config.scale_factor, 0);

  sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);

  return true;
}

bool init_chip8(chip8_t *chip8) {
  const uint32_t entry_point = 0x200;
  const uint8_t font = {
      0
  };
  // Load Font

  // Load ROM

  // Set chip8 Machine defaults
  chip8->state = RUNNING;
  chip8->PC = entry_point;

  return true;
}

void clear_screen(const sdl_t sdl) {
  SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255);
  SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl) { SDL_RenderPresent(sdl.renderer); }

void handle_input(chip8_t *chip8) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      chip8->state = QUIT;
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        chip8->state = QUIT;
        break;
      }
      break;
    case SDL_KEYUP:
      break;
    default:
      break;
    }
  }
}

void final_cleanup(const sdl_t sdl) {
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);

  SDL_Quit();
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // Initialize SDL and cofnig
  sdl_t sdl = {0};
  chip8_t chip8 = {0};
  config_t config = {64, 32, 10};

  if (!init_sdl(&sdl, config)) {
    exit(EXIT_FAILURE);
  }

  if (!init_chip8(&chip8)) {
    exit(EXIT_FAILURE);
  }

  // Initial Screen Clear
  clear_screen(sdl);

  // Main Emulator Loop
  while (chip8.state != QUIT) {
    // Handle User Input
    handle_input(&chip8);

    // CHIP8 Instructions
    // get time since before
    // and substract from this
    SDL_Delay(16 - 0);

    update_screen(sdl);
  }

  // Final Cleanup
  SDL_Log("Exiting Emulator...");
  final_cleanup(sdl);
  exit(EXIT_SUCCESS);
}
