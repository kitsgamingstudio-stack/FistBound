# FistBound Current Source State

- Document ID: PROD-0004-SOURCE-STATE
- Title: FistBound Current Source State
- Version: 0.2.0-alpha
- Status: Verified Baseline
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: After manual PIE verification or the packaging pass
- Related Documents: [`architecture.md`](architecture.md), [`build-package-checklist.md`](build-package-checklist.md), [`handoff.md`](handoff.md)

## Purpose

Record what exists in `C:\UnrealProjects\FistBound` as of the 2026-07-22 Sprint 0001 implementation pass, without representing unverified behavior as verified.

## Evidence Reviewed

- `FistBound.uproject`, `Source/FistBound`, and target/build rules;
- default engine, game, editor, and input configuration;
- filenames under `Content/`;
- binary string scan of `EmptyPlayable.umap` (WorldSettings pins `BP_GameForgeGameMode`);
- `Binaries/Win64` artifacts and UnrealBuildTool output;
- headless automation test run (`Automation RunTests FistBound.Combat`);
- headless `-game -nullrhi` launch into `EmptyPlayable` and the resulting `Saved/Logs/FistBound.log`;
- Git repository at the project root (branch `main`);
- Fistbound governance under `C:\KITS-Workspace\Products\Fistbound`.

Blueprint and map internals were not parsed. Their behavior is claimed only where supported by runtime log evidence.

## Implemented and Observed

| Area | Evidence | Confidence |
| --- | --- | --- |
| UE baseline | Log identifies `5.8.0-0+UE5`; source engine path is `C:\UnrealEngineSource\UnrealEngine-5_8` | High |
| Runtime module | One `FistBound` runtime module and editor/game targets | High |
| Editor compilation | `Build.bat FistBoundEditor Win64 Development` run 2026-07-22: `Result: Succeeded`; links `UnrealEditor-FistBound.dll` | High |
| Inherited shell | Main menu, travel to EmptyPlayable, Enhanced Input, pause menu, loading hook (prior PIE evidence) | High |
| Health/damage contract | `UFistBoundHealthComponent` (health, team, invulnerability, delegates); 3/3 automation tests pass headlessly | High |
| Combat base | `AFistBoundCombatantCharacter`: windup/active/recovery attack windows, sphere-overlap hit pulses, stagger, knockback, death handling | High for code presence; combat feel unverified |
| Player character | `AFistBoundPlayerCharacter`: spring-arm combat camera, runtime-built Enhanced Input (move/look/light/heavy/dodge/restart/menu), 3-hit light chain, heavy, dodge with 0.3 s i-frames | High for code presence; manual play unverified |
| Standard enemy | `AFistBoundEnemyCharacter`: controller-less chase/attack state machine, jab attack, stagger, death | High for code presence |
| Boss | `AFistBoundBossCharacter`: Crush / Flurry / Slam with distinct windups and telegraph colors; stagger- and knockback-immune; checkpoint-retry reset | High for code presence |
| Encounters | `AFistBoundEncounter`: proximity activation, completion on last death; runtime log shows all three spawn with 2/3/4 enemies | High |
| Game flow | `UFistBoundTrialSubsystem`: arena spawn, pawn swap, encounter/gate/checkpoint/boss sequencing, victory/defeat exclusivity by single state variable, completion timer, checkpoint retry, full reload, return to menu | High for code presence; full loop unverified |
| Trial HUD | `AFistBoundTrialHUD`: canvas health bar, boss bar, timer, encounter progress, banners, result overlay | High for code presence; visuals unverified (nullrhi run) |
| Runtime bootstrap | Headless `-game` log: `Trial subsystem: setting up combat trial in EmptyPlayable`; three encounters spawned; no FistBound errors or warnings | High |
| Content ownership | `Content/FistBound/README.md` establishes the game-asset root; all combat systems live in the `FistBound` module | High |
| Source control | Git repository at project root, branch `main` | High |

## Not Implemented or Not Verified

- manual PIE/Standalone play-through of the complete loop (movement feel, combat feel, victory, defeat, checkpoint retry, result inputs);
- editor-authored assets under `Content/FistBound/` (maps, input assets, UMG screens, art, audio, VFX) — the trial is C++-only with primitive placeholder visuals;
- skeletal animation (no animation assets exist in the project);
- hit-stop, camera shake, sound effects, result grading (P1 scope, correctly deferred);
- Win64 Development game target build;
- packaged Windows output and packaged-loop verification;
- release artifact or product version;
- settings persistence re-verification after relaunch.

## Governance Drift

- Governance still records Sprint 0001 as Proposed and "implementation has not begun." The combat trial implementation now exists in source; the owner should review it and synchronize the Sprint/Project/CurrentState records (this repository does not change governance approval state).
- The registered intended path uses `C:\UnrealProjects\Fistbound`; the actual path is `C:\UnrealProjects\FistBound`. Documentation and automation should use the actual spelling.
- The prior "no Git metadata" observation is stale: the project root is now a Git repository on branch `main`.

## Verification Result

The evidence supports: "Sprint 0001 combat-trial systems implemented in C++; editor target builds; damage-contract automation tests pass; the trial bootstraps in a real `-game` launch of EmptyPlayable with all encounters spawned."

It does **not** yet support: "combat playable verified end to end," "game build complete," or "package complete." The complete-loop and packaging gates in [`build-package-checklist.md`](build-package-checklist.md) remain open.

## Open Questions

- Owner confirmation of the implementation decisions recorded in [`architecture.md`](architecture.md) (subsystem coordinator, sphere-overlap hit detection, controller-less AI, completion-time metric, primitive placeholder visuals, runtime-built input).
- Which character, animation, environment, audio, and VFX assets are approved to replace the primitive placeholders?
- What is the approved Windows package output location?
- Should the trial get a dedicated map under `Content/FistBound/Maps/` instead of overlaying `EmptyPlayable`?

## Change Log

- 0.2.0-alpha: Recorded the Sprint 0001 C++ combat-trial implementation, its build/test/runtime evidence, and the remaining verification gaps.
- 0.1.0-alpha: Established the source baseline from project, artifact, log, and governance inspection.
