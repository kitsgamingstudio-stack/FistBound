# FistBound Build and Package Checklist

- Document ID: PROD-0004-SOURCE-VERIFY
- Title: FistBound Build and Package Checklist
- Version: 0.1.0-alpha
- Status: Active Verification Record
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: Every build/package acceptance pass
- Related Documents: [`current-state.md`](current-state.md), [`development.md`](development.md), [`handoff.md`](handoff.md)

## Purpose

Provide one repeatable checklist for validating the inherited shell and, when implemented, the complete FistBound loop. Check an item only when current evidence exists and record the evidence below.

## Environment

- [x] Unreal Engine 5.8.0 opens the project.
- [x] Win64 editor toolchain produces the editor module.
- [ ] Source-control location and workflow are confirmed.
- [ ] Approved third-party or Marketplace asset licenses are recorded.

## Editor Build

- [x] `FistBoundEditor` artifact exists for Win64 Development Editor.
- [x] The project opens `MainMenu`.
- [x] Current startup-map check reports zero errors and zero warnings.
- [x] No fatal error was found in the inspected editor log.
- [ ] A clean/reproducible editor build has been run and recorded.
- [ ] A Win64 Development game target has been built.

## Inherited Foundation Smoke Check

- [x] PIE enters the main menu.
- [x] Start travels to `EmptyPlayable`.
- [x] Enhanced Input initializes.
- [x] Pause menu activation appears in the runtime log.
- [x] Loading-screen hook arms during level travel.
- [ ] Move and look are manually verified.
- [ ] Resume is manually verified.
- [ ] Return to main menu is manually verified.
- [ ] Settings apply and persist after relaunch.
- [ ] Confirm Quit Yes and No branches are manually verified.
- [ ] Keyboard and gamepad CommonUI focus/navigation are verified.
- [ ] Loading-screen presentation is verified in Standalone.

## First-Playable Acceptance

- [ ] Game-specific startup path and `Content/FistBound/` ownership are established.
- [ ] Player movement and combat camera are functional.
- [ ] Light attack, heavy attack, and dodge are functional.
- [ ] Player and standard enemy can damage and defeat each other.
- [ ] Dodge prevents damage only during its approved invulnerability window.
- [ ] Three bounded encounters complete correctly.
- [ ] Boss performs three readable attacks and can be defeated.
- [ ] Checkpoint and restart work.
- [ ] Victory and defeat cannot occur simultaneously.
- [ ] Result UI shows the approved completion metric.
- [ ] Restart and return-to-menu paths work after victory and defeat.

## Windows Package

- [ ] Package configuration and output path are recorded.
- [ ] Win64 package succeeds without blocking cook or stage errors.
- [ ] Packaged executable launches on the target machine.
- [ ] Packaged main-menu and loading flow pass.
- [ ] Packaged complete combat loop passes.
- [ ] Packaged settings persistence passes.
- [ ] Logs contain no unresolved missing-class, missing-asset, or fatal errors.
- [ ] Package location, date, size, build configuration, and verifier are recorded.

## Evidence: 2026-07-22 Documentation Baseline

- Environment: Windows; Unreal Engine log reports `5.8.0-0+UE5` from `C:\UnrealEngineSource\UnrealEngine-5_8`.
- Editor artifact: `Binaries/Win64/UnrealEditor-FistBound.dll`, target receipt, and PDB dated 2026-07-22.
- Runtime log: `Saved/Logs/FistBound.log` records MainMenu PIE startup, Enhanced Input initialization, travel to EmptyPlayable, loading-screen arming, EmptyPlayable entering play, and pause-menu activation.
- Map check: zero errors and zero warnings.
- Negative evidence: no `FistBound.exe`, game target receipt, packaged directory, or gameplay systems were found.
- Verification method: artifact/source/log inspection only; no new build or package command was run.

## Future Evidence Template

- Date:
- Verifier:
- Engine build:
- Source revision or snapshot:
- Build configuration:
- Command or editor action:
- Package path:
- Result:
- Relevant log path:
- Warnings accepted or follow-up tasks:

## Change Log

- 0.1.0-alpha: Recorded inherited-shell evidence and established unchecked first-playable/package acceptance gates.
