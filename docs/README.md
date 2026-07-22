# FistBound Source Documentation

- Document ID: PROD-0004-SOURCE-DOCS
- Title: FistBound Source Documentation Index
- Version: 0.1.0-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: After the next verified implementation change
- Related Documents: [`../README.md`](../README.md), [`../KITS_PRODUCT.md`](../KITS_PRODUCT.md)

## Purpose

Provide implementation-adjacent documentation for contributors working in the FistBound Unreal repository. Product authority, approval, and portfolio status remain in the KITS governance workspace.

## Documents

- [`architecture.md`](architecture.md) — current physical architecture, target responsibilities, dependency rules, and data flow.
- [`current-state.md`](current-state.md) — inspected source/assets/configuration, verification evidence, gaps, and open questions.
- [`development.md`](development.md) — setup, content layout, coding boundaries, and workflow.
- [`build-package-checklist.md`](build-package-checklist.md) — repeatable editor, loop, build, and Windows package checks.
- [`release-history.md`](release-history.md) — explicit unreleased baseline and future release-record requirements.
- [`handoff.md`](handoff.md) — present handoff state and ordered next actions.

## Authority Boundary

Source documentation describes what exists and how to work with it. The following remain authoritative in `C:\KITS-Workspace\Products\Fistbound`:

- product specification and scope;
- product state and readiness;
- architecture approval;
- risks and roadmap;
- project and sprint approval;
- decision records.

If observed behavior conflicts with governance, record the conflict in [`current-state.md`](current-state.md), treat observed implementation as current-state evidence, and obtain owner approval before changing scope or architecture.

## Maintenance Rule

Update the relevant document whenever source behavior, setup, architecture, verification status, or release evidence changes. Do not mark a proposed feature complete without editor or packaged-build evidence appropriate to the claim.

## Change Log

- 0.1.0-alpha: Created the source documentation index and authority boundary.
