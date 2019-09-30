# SkyGameboy

SkyGameboy is another Gameboy emulator. It is a fun project for learning before all.

It have been written using C++17 and code readability and maintainability in mind.

## Frontend and modularity

The emulator itself live in `src/backend`.

There is another folder `src/frontend`. The frontend is a set of interfaces and
implementations which define Display, Input and Audio. You can define your own frontend
as needed by inheriting the interfaces and providing your own implementation. You then
need to update `main.cpp` to use your new frontends.

## Keybindings

Keybindings are:

- `WASD` for Up, Left, Right and Down
- `J` for Button A
- `K` for Button B
- `Enter/return` for START
- `'` for SELECT (It is just left of START)
