Snowballs2 is a demo showcasing the nel networking framework in a simple game.

RuneHarvest's variation of this cleans up and isolates the code outside of the monorepo design of [Ryzom Core](https://github.com/ryzom/ryzomcore).

# Requirements


currently only built for linux

later I plan for stack repo to be the starting point, but for now

- clone this repo, and clone shared up one directory, e.g.:
    - mkdir -p /src/rh
    - cd /src/rh
    - git clone [snowball repo]
    - git clone [shared repo]
    - cd snowball
    - if not using dev container, ln -s ../shared shared

- reopen in dev container (vscode) (this binds shared as a mount point)
- `make init` to copy config/data to build/bin
- ctrl+shift+p `CMake: Configure` with Linux
- ctrl+shift+p `Cmake: Build Target`, select all
- `make run` to run the game



## Controls

Use the mouse to look around.
Use arrow keys to move forward, backward, strafe left and strafe right.

- F3: switch on/off the wireframe mode
- F4: clear the chat window
- F5: switch on/off the chat text
- F6: switch on/off the radar
- F7: zoom out the radar
- F8: zoom in the radar
- F9: capture or uncapture the mouse
- F10: switch betweeon online and offline
- F11: reset player position
- F12: take a screenshot
- SHIFT-ESC: quit
- Left Mouse Button: throw a snowball
- Type text followed by ENTER to broadcast messages

# Copyright

For copyright info see the [LICENSE](LICENSE) and [AUTHORS](AUTHORS).

All assets found within are copyright their respective owners, and can be assumed to be under the same license as the code, unless otherwise specified.

The majority of the code here is also under the same license of AGPLv3, if not all, as per notes below note.
