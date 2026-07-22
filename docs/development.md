# FistBound Development Guide

- Document ID: PROD-0004-SOURCE-DEV
- Title: FistBound Development Guide
- Version: 0.1.1-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: When the gameplay source/content layout changes
- Related Documents: [`architecture.md`](architecture.md), [`build-package-checklist.md`](build-package-checklist.md), [`../README.md`](../README.md)

## Purpose

Define the repeatable local workflow and ownership rules for the FistBound Unreal project.

## Prerequisites

- Windows development machine;
- Unreal Engine 5.8.0 source build;
- Visual Studio 2022 with the C++ workload and Unreal prerequisites;
- access to the KITS governance workspace;
- owner-approved game assets and Sprint 0001 scope before gameplay implementation.

The currently observed engine source is `C:\UnrealEngineSource\UnrealEngine-5_8`.

## Open and Build

1. Read [`../KITS_PRODUCT.md`](../KITS_PRODUCT.md) and the linked governance documents.
2. Open `FistBound.uproject` in Unreal Engine 5.8.0.
3. If project files are stale, regenerate them through Unreal's project-file action for the selected engine build.
4. Build `FistBoundEditor` for Win64 Development Editor.
5. Open `MainMenu`, run PIE, choose Start, and verify travel to `EmptyPlayable`.
6. Record results in [`build-package-checklist.md`](build-package-checklist.md).

Generated folders (`Binaries`, `DerivedDataCache`, `Intermediate`, and `Saved`) are evidence or local outputs, not authoritative source. Do not hand-edit their contents.

## Content Ownership

```text
Content/
  GameForge/         inherited foundation; subclass rather than edit
  Splash/            shared engine-convention splash location
  FistBound/         all new game-specific assets
    Core/
    Gameplay/
    UI/
    Input/
    Maps/
    Art/
      Characters/
      Environments/
      Materials/
      Audio/
      VFX/
```

Creating `Content/FistBound/` is part of the first game-specific implementation task. Fix redirectors after approved asset moves. Never promote an asset into `Content/GameForge/` without a reuse decision and synchronized GameForge governance.

## Code Rules

- Keep the first playable in the existing `FistBound` module.
- Use the `FistBound` prefix for native classes and `FistBound|...` for editor categories.
- Use C++ for stable gameplay contracts, state ownership, and coordination.
- Use Blueprint/UMG for composition, presentation, animation linkage, and tuning.
- Keep combat rules out of widgets and reusable logic out of Level Blueprints.
- Prefer Unreal object references and soft references over string-built asset paths.
- Keep player/enemy damage interactions behind a narrow contract so the boss and HUD do not depend on concrete actor classes.
- Add tests or a documented smoke check with every gameplay system.
- Do not add dependencies, plugins, modules, or shared foundations without approval.

## Trial Controls (current implementation)

Combat input is created at runtime in `AFistBoundPlayerCharacter` (no input
assets exist yet). The inherited GameForge context still provides pause.

| Action | Keyboard/Mouse | Gamepad |
| --- | --- | --- |
| Move | WASD | Left stick |
| Look | Mouse | Right stick |
| Light attack (3-hit chain) | Left mouse button | X / Square |
| Heavy attack | Right mouse button | Y / Triangle |
| Dodge (0.3 s i-frames) | Space or Left Shift | B / Circle |
| Restart / retry (result screens only) | R | A / Cross |
| Return to menu (result screens only) | M | Select / View |
| Pause | Inherited GameForge binding | Inherited GameForge binding |

## Configuration

`DefaultEngine.ini` boots `MainMenu`, uses `UFistBoundGameInstance`, and selects the inherited `BP_GameForgeGameMode`. Preserve the GameForge class/name redirects while inherited Blueprint assets still reference their original parents.

The loading screen is configured with a two-second minimum so it is visible on small maps. Loading screens do not render in PIE; verify them in Standalone or a packaged build.

## Definition of Done for a Change

- implementation matches an approved task and acceptance criteria;
- editor target builds;
- relevant PIE or Standalone smoke checks pass;
- no new unresolved error appears in the log;
- packaged behavior is checked when the change affects loading, cooking, assets, input, or shipping flow;
- source docs and KITS governance are synchronized where their facts changed;
- incomplete or unverified work is labelled explicitly.

## Change Log

- 0.1.1-alpha: Documented the implemented trial controls.
- 0.1.0-alpha: Created the source/content workflow and contribution boundaries.
