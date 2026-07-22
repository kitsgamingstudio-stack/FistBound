# FistBound

- Document ID: PROD-0004-SOURCE-README
- Title: FistBound Source Repository
- Version: 0.2.0-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: Before Sprint 0001 implementation approval
- Related Documents: [`KITS_PRODUCT.md`](KITS_PRODUCT.md), [`docs/README.md`](docs/README.md)

## Purpose

FistBound is a compact third-person martial-arts action game derived from the KITS GameForge Unreal Engine foundation. The proposed first release is a seven-day playable containing a short combat trial, one standard enemy archetype, three encounters, and one boss.

This repository contains the inherited application shell plus the Sprint 0001 combat trial implemented in C++ (player combat, one standard enemy, three encounters, checkpoint, one boss, victory/defeat/result flow, completion timer). The trial is editor-verified but not yet human-play-tested or packaged.

## Status

| Field | Current value |
| --- | --- |
| Product ID | `PROD-0004` |
| Product state | Sprint 0001 combat trial implemented; verification and packaging pending |
| Engine | Unreal Engine 5.8.0 source build |
| Initial platform | Windows |
| Source path | `C:\UnrealProjects\FistBound` |
| Source control | Git repository, branch `main` |
| Distribution | Unreleased |
| Current product version | Unreleased |

Observed on 2026-07-22:

- the `FistBoundEditor` module builds (`Result: Succeeded`);
- damage-contract automation tests pass headlessly (3/3);
- a headless `-game` launch of `EmptyPlayable` bootstraps the trial and spawns all three encounters with no FistBound errors;
- the inherited main-menu / travel / pause / loading shell remains intact and untouched;
- no game executable or packaged Windows build exists yet;
- no editor-authored game assets exist yet — the trial uses primitive placeholder visuals, runtime-built input, and a canvas HUD.

See [`docs/current-state.md`](docs/current-state.md) for the evidence boundary and [`docs/development.md`](docs/development.md) for the trial controls.

## Repository Map

```text
Config/              Unreal project and platform configuration
Content/GameForge/   Inherited foundation assets; subclass, do not edit in place
Content/Splash/      Engine splash assets
Source/FistBound/    Renamed foundation C++ module
.kits/               Machine-readable KITS discovery metadata
docs/                Source-repository documentation and verification records
```

Game-specific assets must be created under `Content/FistBound/`. Game-specific C++ remains in the single `FistBound` runtime module unless an approved architecture decision establishes another boundary.

## Getting Started

1. Use the Unreal Engine 5.8.0 source build at `C:\UnrealEngineSource\UnrealEngine-5_8`, or regenerate project files for another owner-approved 5.8.0 build.
2. Open `FistBound.uproject`.
3. Allow Unreal Build Tool to compile the `FistBoundEditor` target when required.
4. Run PIE from `MainMenu` and verify Start travels to `EmptyPlayable`.
5. Follow [`docs/build-package-checklist.md`](docs/build-package-checklist.md) before claiming a build or package is complete.

## Documentation

- [`KITS_PRODUCT.md`](KITS_PRODUCT.md) — product identity and governance pointer.
- [`.kits/product.json`](.kits/product.json) — machine-readable product metadata.
- [`docs/README.md`](docs/README.md) — documentation index.
- [`docs/architecture.md`](docs/architecture.md) — implemented and proposed architecture boundaries.
- [`docs/current-state.md`](docs/current-state.md) — inspected implementation baseline and gaps.
- [`docs/development.md`](docs/development.md) — source/content rules and developer workflow.
- [`docs/build-package-checklist.md`](docs/build-package-checklist.md) — editor, gameplay, and package verification.
- [`docs/release-history.md`](docs/release-history.md) — explicit unreleased history.
- [`docs/handoff.md`](docs/handoff.md) — current handoff and ordered next actions.

Authoritative product governance is maintained at `C:\KITS-Workspace\Products\Fistbound\README.md`. When implementation changes product status, scope, architecture, risks, or sprint evidence, update that governance record as part of the same handoff.

## Scope Guardrails

The initial playable excludes multiplayer, online services, inventory, loot, skill trees, multiple characters or weapons, procedural levels, open-world systems, tower defense, kingdom strategy, and Action Foundation extraction. Reduce or defer features before extending the seven-day boundary.

## Change Log

- 0.2.0-alpha: Updated status for the Sprint 0001 C++ combat-trial implementation pass.
- 0.1.0-alpha: Created the source-repository index from KITS governance and inspected project evidence.
