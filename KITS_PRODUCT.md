# KITS Product Anchor: FistBound

- Document ID: PROD-0004-SOURCE-ANCHOR
- Title: FistBound KITS Product Anchor
- Version: 0.1.0-alpha
- Status: Active Draft
- Owner: Michael Knight
- Last Updated: 2026-07-22
- Next Review: Before Sprint 0001 implementation approval
- Related Documents: [`README.md`](README.md), [`docs/README.md`](docs/README.md)

## Purpose

Identify this Unreal project as the source location for KITS product `PROD-0004` and provide a durable path from source to authoritative governance.

## Product Identity

- Product name: Fistbound (governance working title)
- Unreal project/module name: FistBound
- Product ID: `PROD-0004`
- Owner: Michael Knight
- Company: Knight Integrated Technology Solutions
- Parent capability: KITS GameForge (`PROD-0003`)
- Engine: Unreal Engine 5.8.0
- Source location: `C:\UnrealProjects\FistBound`
- Governance workspace: `C:\KITS-Workspace`
- Governance index: `C:\KITS-Workspace\Products\Fistbound\README.md`

The capitalization difference between the governance title (`Fistbound`) and Unreal identifier (`FistBound`) is preserved intentionally. Rename neither without an approved migration decision.

## Current Authorization Boundary

KITS governance records the product as Proposed, in Planning, and not ready pending approvals and source discovery anchors. This repository proves that derivation/bootstrap work already exists, but it does not by itself approve the proposed combat sprint or change the Product Readiness Review outcome.

Implementation must remain within an approved project, sprint, and task. FistBound owns game-specific action systems; GameForge must not depend on FistBound or absorb unproven combat abstractions.

## Governance Reading Order

1. `C:\KITS-Workspace\Products\Fistbound\README.md`
2. `C:\KITS-Workspace\Products\Fistbound\Product\ProductSpecification.md`
3. `C:\KITS-Workspace\Products\Fistbound\Architecture\ArchitectureSummary.md`
4. `C:\KITS-Workspace\Products\Fistbound\Product\CurrentState.md`
5. `C:\KITS-Workspace\Products\Fistbound\Projects\FistboundInitialGame.md`
6. `C:\KITS-Workspace\Products\Fistbound\Sprints\Sprint-0001-WeeklyPlayable.md`
7. [`docs/current-state.md`](docs/current-state.md)

## Required Discovery Anchors

- [`README.md`](README.md)
- [`KITS_PRODUCT.md`](KITS_PRODUCT.md)
- [`.kits/product.json`](.kits/product.json)
- [`docs/README.md`](docs/README.md)

## Change Log

- 0.1.0-alpha: Created the source-to-governance discovery anchor and recorded the authorization boundary.
