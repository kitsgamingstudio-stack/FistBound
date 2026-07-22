# FistBound Architecture

- Document ID: PROD-0004-SOURCE-ARCH
- Title: FistBound Implemented and Target Architecture
- Version: 0.1.0-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: Before adding the first combat contract
- Related Documents: [`current-state.md`](current-state.md), [`development.md`](development.md), [`../KITS_PRODUCT.md`](../KITS_PRODUCT.md)

## Purpose

Describe the physical architecture observed in the source repository and separate it from the proposed architecture for the first playable.

## Current System Boundary

The project is a GameForge-derived Unreal Engine 5.8 application shell with one runtime module.

```text
Unreal Engine 5.8
  +-- Enhanced Input
  +-- CommonUI
  +-- UMG / Slate / MoviePlayer
          |
          v
FistBound runtime module
  +-- GameInstance: loading-screen lifecycle
  +-- PlayerController: menu, pause, input modes, level travel
  +-- UISubsystem: CommonUI menu stack
  +-- SaveSubsystem + SaveGame: volume and graphics settings
  +-- GameModeBase + DefaultPawn: possession and basic move/look shell
          |
          v
Inherited Content/GameForge assets
  +-- MainMenu, EmptyPlayable, LoadingScreen maps
  +-- Core Blueprint subclasses
  +-- CommonUI layouts and menus
  +-- Enhanced Input actions and mapping context
```

### Physical Dependencies

The public module depends on `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `UMG`, and `CommonUI`. Private dependencies are `MoviePlayer`, `Slate`, and `SlateCore`. The project enables CommonUI and the editor-only Modeling Tools mode.

### Implemented Flow

```text
MainMenu map
  -> PlayerController / UI stack
  -> StartGame
  -> OpenLevel(EmptyPlayable)
  -> loading-screen hook
  -> possessed default pawn
  -> Pause / settings / return-to-menu shell
```

Settings use one local save slot named `FistBoundSettings`. The payload contains master volume, overall graphics quality, last-level name, and save version. No gameplay progression is implemented.

## Target First-Playable Responsibilities

These responsibilities are proposed by KITS governance but are not implemented as of 2026-07-22:

- game-flow coordination for trial, encounters, checkpoint, boss, victory, defeat, restart, and results;
- third-person action character with light attack, heavy attack, dodge, health, and animation requests;
- hit detection, damage, attack windows, invulnerability, reactions, death, and target filtering;
- one standard enemy archetype;
- one boss with three readable attacks;
- combat HUD, timer or score, result presentation, and restart actions;
- game-specific maps, input, UI, art, audio, and effects under `Content/FistBound/`.

## Dependency Rules

1. Keep one `FistBound` runtime module during the first playable unless measured coupling or build constraints justify an approved split.
2. Keep all action-game code and assets local to FistBound.
3. Do not modify GameForge to absorb FistBound combat mechanics.
4. Treat `Content/GameForge/` as inherited foundation content; subclass it into `Content/FistBound/`.
5. UI may observe and present gameplay state but must not own combat rules.
6. Avoid reusable gameplay logic in Level Blueprints.
7. Prefer C++ for stable contracts and coordination; use Blueprint/UMG for presentation, animation wiring, level composition, and tuning.
8. Add no runtime plugin, framework, or module without an explicit decision and demonstrated need.

## Proposed Gameplay Data Flow

```text
Enhanced Input
  -> FistBound character action state
  -> animation / attack window
  -> hit query
  -> damage receiver and health
  -> reaction or death signal
  -> encounter/game-flow coordinator
  -> HUD and result presentation
```

## Technical Concerns

- The C++ classes and module are named FistBound, while inherited assets remain named and located under GameForge. This is acceptable during bootstrap but makes ownership easy to blur.
- `DefaultEngine.ini` contains GameForge-to-FistBound redirects and duplicate renderer keys. Preserve redirects until asset migration is verified; clean duplicates only through a separately tested configuration change.
- Hardware ray tracing, Lumen, Nanite, virtual textures, and Substrate are enabled. Their cost and minimum hardware fit have not been validated for the compact game.
- The CommonUI runtime log reports focus fallback diagnostics for the pause menu. This is non-blocking in the observed PIE session but should be checked for keyboard/gamepad navigation.
- There are no automated gameplay tests.

## Change Log

- 0.1.0-alpha: Documented the inspected foundation architecture and separated it from the proposed first-playable systems.
