# FistBound Current Source State

- Document ID: PROD-0004-SOURCE-STATE
- Title: FistBound Current Source State
- Version: 0.1.0-alpha
- Status: Verified Baseline
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: After the next implementation or owner verification pass
- Related Documents: [`architecture.md`](architecture.md), [`build-package-checklist.md`](build-package-checklist.md), [`handoff.md`](handoff.md)

## Purpose

Record what was observed in `C:\UnrealProjects\FistBound` on 2026-07-22 without representing proposals as completed work.

## Evidence Reviewed

- `FistBound.uproject`;
- `Source/FistBound` and target/build rules;
- default engine, game, editor, and input configuration;
- filenames under `Content/`;
- `Binaries/Win64` artifacts;
- the latest `Saved/Logs/FistBound.log`;
- Git discovery from the project root;
- Fistbound governance under `C:\KITS-Workspace\Products\Fistbound`;
- KITS documentation, versioning, repository, quality, and onboarding standards;
- GameForge derived-game documentation and verification conventions.

Blueprint and map internals were not parsed. Their behavior is claimed only where supported by runtime log evidence.

## Implemented and Observed

| Area | Evidence | Confidence |
| --- | --- | --- |
| UE baseline | Log identifies `5.8.0-0+UE5`; source engine path is `C:\UnrealEngineSource\UnrealEngine-5_8` | High |
| Runtime module | One `FistBound` runtime module and editor/game targets | High |
| Editor compilation | `UnrealEditor-FistBound.dll`, PDB, and target receipt dated 2026-07-22 | High |
| Editor startup | Main menu map opens; map check reports 0 errors and 0 warnings | High |
| Menu-to-game travel | PIE log records MainMenu, Browse/OpenLevel to EmptyPlayable, and both worlds entering play | High |
| Input | Enhanced Input initializes; move/look/pause contracts exist | High |
| Pause UI | PIE log records `WBP_PauseMenu` becoming the active CommonUI node | High |
| Loading hook | Log records the FistBound loading screen arming for EmptyPlayable | High |
| Settings/save shell | C++ implementation provides local settings load/save/apply | High for code presence; runtime persistence not reverified |
| Content shell | 16 `.uasset` files, 3 `.umap` files, and 4 splash images; all non-splash assets remain under `Content/GameForge/` | High |

## Not Implemented or Not Found

- `Content/FistBound/` game-specific assets;
- martial-arts player character or combat camera;
- light attack, heavy attack, dodge, attack windows, or invulnerability;
- health/damage contract used by combat actors;
- standard enemy AI;
- boss AI or attack patterns;
- encounter, checkpoint, victory, defeat, restart, timer, score, or result systems;
- gameplay automation tests;
- Windows game executable;
- packaged Windows output;
- release artifact or product version;
- Git repository metadata at or above the project root.

## Governance Drift

The KITS product baseline says the source repository is not yet created and that implementation has not begun. The directory and editor build now exist, so those statements are stale. Governance remains authoritative for approval state, but its current-state evidence should be updated after owner review.

The registered intended path uses `C:\UnrealProjects\Fistbound`; the actual path is `C:\UnrealProjects\FistBound`. Windows treats these as the same path in ordinary use, but documentation and automation should use the actual spelling.

The KITS onboarding package does not currently include a Release History document even though KDM-002 lists one as a required deliverable. This repository adds [`release-history.md`](release-history.md); governance should add or link an authoritative release record.

## Verification Result

The inspected evidence supports “GameForge-derived application shell bootstrapped and editor-loaded.” It does not support “Sprint 0001 started,” “combat playable,” “game build complete,” or “package complete.”

No new compile or package command was run during this documentation review. Existing artifacts and logs were inspected non-destructively.

## Open Questions

- Has the owner approved the title, architecture boundary, project, and Sprint 0001 outside the recorded governance documents?
- Which character, animation, environment, audio, and VFX assets are approved for the weekly playable?
- Which hit-detection approach and AI approach will be used?
- Is completion time, score, or both required for Sprint 0001?
- Should Git be initialized here, or is source control managed from another root?
- What is the approved Windows package output location?

## Change Log

- 0.1.0-alpha: Established the source baseline from project, artifact, log, and governance inspection.
