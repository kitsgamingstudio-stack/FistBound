# FistBound Build and Package Checklist

- Document ID: PROD-0004-SOURCE-VERIFY
- Title: FistBound Build and Package Checklist
- Version: 0.2.0-alpha
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
- [x] Source-control location and workflow are confirmed. (Git repository at the project root, branch `main`; workflow per [`development.md`](development.md).)
- [ ] Approved third-party or Marketplace asset licenses are recorded. (None used; trial uses engine basic shapes only.)

## Editor Build

- [x] `FistBoundEditor` artifact exists for Win64 Development Editor.
- [x] The project opens `MainMenu`.
- [x] Current startup-map check reports zero errors and zero warnings.
- [x] No fatal error was found in the inspected editor log.
- [x] A clean/reproducible editor build has been run and recorded. (2026-07-22 implementation evidence below.)
- [x] A Win64 Development game target has been built. (`FistBound.exe`, 2026-07-22 package evidence below.)

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

Implemented in C++ on 2026-07-22. Checked items are owner-play-verified
(2026-07-22 first play pass); the rest remain open until observed in play.

- [x] Game-specific startup path and `Content/FistBound/` ownership are established. (Trial subsystem bootstraps in `EmptyPlayable`; `Content/FistBound/README.md` establishes the asset root.)
- [x] Player movement and combat camera are functional. (Owner first play pass.)
- [x] Light attack, heavy attack, and dodge are functional. (Owner first play pass.)
- [x] Player and standard enemy can damage and defeat each other. (Encounters cleared; player death and retry observed.)
- [ ] Dodge prevents damage only during its approved invulnerability window. (Automation verifies the invulnerability contract; the in-play timing window is not yet deliberately tested.)
- [x] Three bounded encounters complete correctly. (All three cleared; gate opened; checkpoint reached.)
- [ ] Boss performs three readable attacks and can be defeated. (Boss fought; a victory has not yet been reported.)
- [x] Checkpoint and restart work. (Defeat followed by checkpoint retry exercised.)
- [ ] Victory and defeat cannot occur simultaneously. (Guaranteed by the single state variable in code; victory not yet observed in play.)
- [ ] Result UI shows the approved completion metric. (Defeat result with time observed; victory result pending.)
- [ ] Restart and return-to-menu paths work after victory and defeat. (Post-defeat retry verified; post-victory paths and return-to-menu pending.)

## Windows Package

- [x] Package configuration and output path are recorded. (Development config; `Saved/Packages/Windows` — provisional location pending owner approval.)
- [x] Win64 package succeeds without blocking cook or stage errors. (`BUILD SUCCESSFUL`, 2026-07-22.)
- [x] Packaged executable launches on the target machine. (Two headless boots on the build machine; see evidence.)
- [ ] Packaged main-menu and loading flow pass. (Menu boots and the loading screen arms in the headless log; interactive Start-click travel and visible loading-screen presentation still need a human run.)
- [ ] Packaged complete combat loop passes. (Trial bootstraps and all encounters spawn in the packaged trial-map boot; the full played loop needs a human run.)
- [ ] Packaged settings persistence passes.
- [x] Logs contain no unresolved missing-class, missing-asset, or fatal errors. (Both packaged boot logs scanned clean; the known CommonUI focus-fallback diagnostic persists as documented.)
- [x] Package location, date, size, build configuration, and verifier are recorded. (See evidence below.)

## Evidence: 2026-07-22 Windows Package Pass

- Date: 2026-07-22
- Verifier: Claude Code (automated pass), following the owner's confirmed first play pass
- Engine build: `5.8.0` source build at `C:\UnrealEngineSource\UnrealEngine-5_8`
- Source revision or snapshot: working tree on branch `main` (uncommitted at verification time)
- Build configuration: Win64 Development
- Commands:
  - `Build.bat FistBound Win64 Development` → `Result: Succeeded` (links `Binaries/Win64/FistBound.exe`; first monolithic build, ~23 min)
  - `RunUAT.bat BuildCookRun -platform=Win64 -clientconfig=Development -build -cook -stage -pak -package -archive` → `BUILD SUCCESSFUL` (93 s; warm DDC)
- Package path: `Saved/Packages/Windows` (provisional; approved output location is an open question)
- Package size: 3,183.2 MB (Development config with debug files; Shipping would be smaller)
- Cook stats: 548 packages, 0 cook errors; UnrealPak success
- Packaged boot 1 (no args, `-nullrhi -unattended`): boots to `MainMenu`, loading screen arms, `WBP_MainMenu` activates; no fatal errors
- Packaged boot 2 (`EmptyPlayable -nullrhi -unattended`): trial subsystem sets up, encounters spawn 2/3/4 enemies; engine BasicShapes cooked correctly; no missing-asset or fatal errors
- Relevant log path: `Saved/Packages/Windows/FistBound/Saved/Logs/`
- Warnings accepted or follow-up tasks: human play of the packaged loop (menu → trial → result), packaged settings persistence, visible loading-screen check, and the pre-existing CommonUI focus-fallback diagnostic

## Evidence: 2026-07-22 Owner First Play Pass

- Date: 2026-07-22
- Verifier: Michael Knight (owner), in-editor play
- Reported result: "first pass is success"
- Scope reported: movement, camera, attacks, and dodge functional; all three encounters cleared; defeat and checkpoint retry exercised
- Not yet exercised: boss victory, victory result screen, post-victory restart, return-to-menu from a result screen, deliberate i-frame timing test
- Checklist items checked above reflect exactly this scope

## Evidence: 2026-07-22 Sprint 0001 Implementation Pass

- Date: 2026-07-22
- Verifier: Claude Code (implementation pass), on owner instruction to implement from the docs
- Engine build: `5.8.0` source build at `C:\UnrealEngineSource\UnrealEngine-5_8`
- Source revision or snapshot: working tree on branch `main` (uncommitted at verification time)
- Build configuration: Win64 Development Editor
- Command or editor action: `Build.bat FistBoundEditor Win64 Development -Project=...\FistBound.uproject` → `Result: Succeeded`
- Automation tests: `UnrealEditor-Cmd ... -ExecCmds="Automation RunTests FistBound.Combat; Quit" -nullrhi` → 3/3 Success (`DamageAndDeath`, `InvulnerabilityWindow`, `ResetForRetry`), exit code 0
- Runtime smoke: `UnrealEditor-Cmd ... /Game/GameForge/Maps/EmptyPlayable -game -nullrhi -unattended` → log records `Trial subsystem: setting up combat trial in EmptyPlayable` and encounters spawning 2/3/4 enemies; no FistBound errors or warnings
- Package path: none (packaging not attempted)
- Result: implementation compiles, unit tests pass, trial bootstraps in a real game launch
- Relevant log path: `Saved/Logs/FistBound.log`
- Warnings accepted or follow-up tasks: manual play-through of the complete loop, game-target build, and Windows packaging remain open

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

- 0.2.0-alpha: Recorded the Sprint 0001 implementation-pass evidence (editor build, automation tests, runtime bootstrap smoke) and checked the items it supports.
- 0.1.0-alpha: Recorded inherited-shell evidence and established unchecked first-playable/package acceptance gates.
