# Space Survivors

A modern remake of the classic arcade game *Asteroids*, built in C using [Raylib](https://www.raylib.com/). Pilot your spaceship, destroy waves of enemies, and survive as long as possible while leveling up and improving your ship.

## Table of Contents

* [Features](#features)
* [Controls](#controls)
* [Installation](#installation)
* [Running the Game](#running-the-game)
* [Assets](#assets)
* [Dependencies](#dependencies)
* [License](#license)

---

## Features

* Classic top-down space shooter gameplay.
* Multiple waves of enemies with increasing difficulty.
* Player leveling system with upgrades:

  * Increase Max Health
  * Increase Projectile Count
  * Increase Movement Speed
* Particle effects for explosions and thrusters.
* Background stars with dynamic movement.
* Pause menu, controls screen, and game over screen.
* Background music and sound effects.

## Controls

* **W** – Move forward / accelerate
* **A** – Rotate left
* **D** – Rotate right
* **SPACE** – Shoot projectiles
* **ESC** – Pause / Back / Menu navigation

## Installation

1. Clone the repository:

```bash
git clone https://github.com/yourusername/space-survivors.git
cd space-survivors
```

2. Ensure that all assets are in the `assets/` folder:

```
assets/
-enemy.png
-ship.png
-sfx/
 --explosion.mp3
 --thruster.mp3
 --button.mp3
-music/
 --menu.mp3
 --gameplay.mp3
```

3. Compile the game using GCC:

```bash
gcc src/ss.c -I./src -std=c11 -Wall -Wextra -lraylib -lopengl32 -lgdi32 -lwinmm -o space-survivors.exe
```

## Running the Game

Run the executable from the src folder:

```bash
../space-survivors.exe
```

> **Note:** The game will fail to load assets if the executable is run from a directory that does not contain the `assets/` folder.

## Dependencies

* [Raylib](https://www.raylib.com/) – Simple and easy-to-use library to enjoy videogames programming.
* Standard C libraries: `stdio.h`, `math.h`.
* Windows libraries (for linking): `opengl32`, `gdi32`, `winmm`.

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.