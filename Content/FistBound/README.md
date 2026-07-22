# Content/FistBound

Ownership root for all game-specific FistBound assets, per
[`docs/development.md`](../../docs/development.md).

The Sprint 0001 combat trial is currently implemented entirely in C++
(runtime-built arena, primitive placeholder visuals, runtime-created input
actions, canvas HUD), so no `.uasset` files exist here yet. When editor-authored
assets are approved and produced, they belong under:

```text
FistBound/
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

Do not add or promote assets into `Content/GameForge/`; subclass inherited
foundation assets into this tree instead.
