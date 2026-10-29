# Strange Things at Starling Hill — SNES homebrew

This folder contains the first native SNES build of the game. It is a deliberately tiny prototype using PVSnesLib's text console to prove the controller/game loop and ROM build pipeline before adding custom tile graphics and sprite art.

## Controls
- D-pad: move
- A: jump
- B: interact with mailbox / hill entrance

## Build
The GitHub Actions workflow in `.github/workflows/build-snes.yml` builds PVSnesLib and compiles `starling_hill.sfc`, then publishes it as a downloadable workflow artifact. It runs on pushes and can be started manually from the Actions tab.

This is an early prototype, not the finished pixel-art version. The next milestone after a successful build is to replace the text glyphs with SNES tiles and a player sprite.

## Play on iPhone
1. Download `starling-hill-snes-rom` from the successful GitHub Actions run.
2. Save the downloaded artifact ZIP to Files and unzip it.
3. In Delta, tap **+** → **Files**, then choose `starling_hill.sfc`.
4. The SNES ROM formats accepted by Delta include `.sfc`, `.smc`, and `.fig`.
