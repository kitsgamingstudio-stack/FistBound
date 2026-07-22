# FistBound Architecture

- Document ID: PROD-0004-SOURCE-ARCH
- Title: FistBound Implemented and Target Architecture
- Version: 0.2.0-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: After owner review of the Sprint 0001 implementation pass
- Related Documents: [`current-state.md`](current-state.md), [`development.md`](development.md), [`../KITS_PRODUCT.md`](../KITS_PRODUCT.md)

## Purpose

Describe the physical architecture observed in the source repository, including the Sprint 0001 combat-trial systems implemented in C++ on 2026-07-22.

## Current System Boundary

The project is a GameForge-derived Unreal Engine 5.8 application shell with one runtime module. The same module now also contains the first-playable combat trial.

```text
Unreal Engine 5.8
  +-- Enhanced Input
  +-- CommonUI
  +-- UMG / Slate / MoviePlayer
          |
          v
FistBound runtime module
  |
  +-- Inherited foundation (unchanged)
  |     +-- GameInstance: loading-screen lifecycle
  |     +-- PlayerController: menu, pause, input modes, level travel
  |     +-- UISubsystem: CommonUI menu stack
  |     +-- SaveSubsystem + SaveGame: volume and graphics settings
  |     +-- GameModeBase + DefaultPawn: possession and basic move/look shell
  |
  +-- Combat trial (Sprint 0001 implementation)
        +-- FistBoundTrialSubsystem: world subsystem game-flow coordinator
        +-- FistBoundHealthComponent: narrow health/damage/team contract
        +-- FistBoundCombatantCharacter: attack windows, hit query, stagger, death
        |     +-- FistBoundPlayerCharacter: camera, input, light chain, heavy, dodge
        |     +-- FistBoundEnemyCharacter: tick state-machine AI
        |           +-- FistBoundBossCharacter: three telegraphed attacks, retry reset
        +-- FistBoundEncounter: bounded spawn/activate/complete groups
        +-- FistBoundTrialHUD: canvas presentation (reads state only)
          |
          v
Inherited Content/GameForge assets (unchanged)
  +-- MainMenu, EmptyPlayable, LoadingScreen maps
  +-- Core Blueprint subclasses
  +-- CommonUI layouts and menus
  +-- Enhanced Input actions and mapping context
```

### Physical Dependencies

The public module depends on `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `UMG`, and `CommonUI`. Private dependencies are `MoviePlayer`, `Slate`, and `SlateCore`. The project enables CommonUI and the editor-only Modeling Tools mode. The combat trial added **no** modules, plugins, AIController, behavior tree, or navmesh dependencies.

### Implemented Flow

```text
MainMenu map
  -> PlayerController / UI stack
  -> StartGame
  -> OpenLevel(EmptyPlayable)
  -> loading-screen hook
  -> FistBoundTrialSubsystem (map-name match on EmptyPlayable)
       -> spawns floating trial arena above the inherited shell level
       -> swaps foundation pawn for FistBoundPlayerCharacter, sets trial HUD
       -> three proximity-activated encounters (2/3/4 enemies)
       -> gate opens + checkpoint after encounter 3
       -> boss engagement in the arena ring
       -> Victory | Defeat -> result overlay -> restart / retry / menu
  -> Pause / settings / return-to-menu shell (inherited, still active)
```

Settings use one local save slot named `FistBoundSettings` (unchanged).

## Sprint 0001 Implementation Decisions

Recorded here for governance synchronization; the owner should confirm or amend each.

1. **Game-flow coordinator is a world subsystem, not a GameMode.** `EmptyPlayable.umap` pins `BP_GameForgeGameMode` in its WorldSettings, and inherited GameForge assets must not be edited. `UFistBoundTrialSubsystem` activates by short map name (config-overridable `TrialMapName`) and coordinates everything, leaving all GameForge assets and config wiring untouched.
2. **Hit detection: timed sphere overlap.** Attacks run windup -> active -> recovery on timers; during the active window a repeating sphere overlap in front of the attacker (or centered, for the boss slam) applies damage through the health contract. Smallest workable approach; anim-notify hitboxes can replace it when animation assets exist.
3. **Enemy AI: controller-less tick state machine.** Enemies use `bRunPhysicsWithNoController` and steer with direct movement input on the flat arena. No AIController, behavior tree, or navmesh. Chosen as the smallest approach for a corridor arena.
4. **Completion metric: completion time.** The trial clock accumulates while fighting, freezes on the result states, and continues across checkpoint retries (deaths cost time). Score is deferred.
5. **Placeholder visuals: engine primitive shapes.** No animation, character, or environment assets are owned/approved yet, so combatants are color-coded primitive rigs and telegraphs are body-tint plus fist-scale pulses during windup. The arena is spawned at runtime from engine cube meshes, floated above the inherited level to avoid touching or depending on its geometry.
6. **Combat input is created at runtime in C++.** Light/heavy/dodge/restart/menu input actions and their keyboard/gamepad mappings are built in `FistBoundPlayerCharacter` at possession time (priority 1 context layered over the inherited pause context), because game-specific input assets cannot be authored without the editor. Replace with `Content/FistBound/Input/` assets later.
7. **HUD is canvas-drawn C++.** `AFistBoundTrialHUD` reads the subsystem and health components and owns no combat rules. UMG/CommonUI screens under `Content/FistBound/UI/` should replace it during the polish pass.

## Dependency Rules

1. Keep one `FistBound` runtime module during the first playable unless measured coupling or build constraints justify an approved split.
2. Keep all action-game code and assets local to FistBound.
3. Do not modify GameForge to absorb FistBound combat mechanics.
4. Treat `Content/GameForge/` as inherited foundation content; subclass it into `Content/FistBound/`.
5. UI may observe and present gameplay state but must not own combat rules. (`AFistBoundTrialHUD` complies: read-only.)
6. Avoid reusable gameplay logic in Level Blueprints. (No Level Blueprint logic was added.)
7. Prefer C++ for stable contracts and coordination; use Blueprint/UMG for presentation, animation wiring, level composition, and tuning.
8. Add no runtime plugin, framework, or module without an explicit decision and demonstrated need. (None added.)
9. Combat actors depend on `UFistBoundHealthComponent` (health, team, invulnerability) rather than concrete character classes; the HUD and encounters bind to its delegates.

## Implemented Gameplay Data Flow

```text
Enhanced Input (runtime-built trial context)
  -> FistBoundPlayerCharacter action state (chain / heavy / dodge i-frames)
  -> attack window timers
  -> sphere-overlap hit query (team-filtered)
  -> UFistBoundHealthComponent (damage, invulnerability, death delegates)
  -> victim stagger / knockback / death
  -> FistBoundEncounter completion -> FistBoundTrialSubsystem game flow
  -> FistBoundTrialHUD presentation
```

## Technical Concerns

- The C++ classes and module are named FistBound, while inherited assets remain named and located under GameForge. This is acceptable during bootstrap but makes ownership easy to blur.
- `DefaultEngine.ini` contains GameForge-to-FistBound redirects and duplicate renderer keys. Preserve redirects until asset migration is verified; clean duplicates only through a separately tested configuration change.
- Hardware ray tracing, Lumen, Nanite, virtual textures, and Substrate are enabled. Their cost and minimum hardware fit have not been validated for the compact game.
- The CommonUI runtime log reports focus fallback diagnostics for the pause menu. This is non-blocking in the observed PIE session but should be checked for keyboard/gamepad navigation.
- The trial arena floats at Z+2600 above the inherited `EmptyPlayable` geometry. A fall-recovery sweep teleports the player back and culls fallen enemies, but arena bounds are placeholder blocks, not authored level design.
- The trial subsystem keys off the map short name `EmptyPlayable`. If the gameplay map is renamed or a dedicated `Content/FistBound/Maps/` map is created, update the `TrialMapName` config value.
- Automated tests cover the health/damage contract only (`Source/FistBound/Tests/`); encounter, boss, and flow logic are verified by the documented smoke checks in [`build-package-checklist.md`](build-package-checklist.md).

## Change Log

- 0.2.0-alpha: Recorded the implemented Sprint 0001 combat-trial architecture and its implementation decisions.
- 0.1.0-alpha: Documented the inspected foundation architecture and separated it from the proposed first-playable systems.
