# Siege Lab — Gameplay Unit Tests (55 cases)

## Test rules

Use a fresh fixture for every case. Avoid a raylib window or audio device: test `draw()` dispatch with a headless subclass and sound requests with a command sink. Verify graphics and listening manually in `SiegeLabGame`.

- Use `dt=1/60 s` unless specified; `+Y` up and `+X` forward.
- Assert IDs, counts and state transitions exactly; use the tolerance stated per floating-point assertion.
- Link **the real teacher-pinned Cyclone** for tests that claim solver or detector behavior.
- No `dynamic_cast`, `std::variant` or `std::visit`; clear registrations, listeners and pair history between tests.

## Group map

The prefixes map test cases to the owning milestone. Keep the IDs so a failed assertion points directly to the correct assignment step.

| Group | Test IDs | Count | Gameplay milestone |
|---|---|---:|---|
| Generic pool | `P01–P05` | 5 | Cross-cutting pool |
| Entity composition, inheritance and factory | `GO01–GO06` | 6 | Pool + GP-M3 / GP-M5 |
| Orbital camera and replaceable camera controller | `C01–C05; CAM06` | 6 | GP-M1 |
| Launcher and shot kind | `L01–L05` | 5 | GP-M2 |
| Real Cyclone projectile | `PR01–PR05` | 5 | GP-M3 |
| Ghost trajectory via Cyclone | `T01–T04` | 4 | GP-M4 |
| Pillar and top cube | `CO01–CO03` | 3 | GP-M5 |
| Single solid CageBlock and direct destruction | `CA01–CA06` | 6 | GP-M8 |
| Indirect chain reaction | `CH01–CH03` | 3 | GP-M9 |
| State machine / lifecycle / reset | `GS01–GS09` | 9 | GP-M10 |
| Audio commands, no audio device | `AU01–AU03` | 3 | GP-M11 |
| **TOTAL** | | **55** | |

## GP-T1 — Generic pool (5 tests)

The pool is pure C++20. Verify capacity, generation handles and stable storage without Cyclone or raylib.

### P01 · `Pool.CapacityLimit`

- **Prepare:** Capacity `3`; values `10`, `20`, `30`, then `40`.
- **Run:** Attempt to create four objects.
- **Check:** Three valid IDs for `10/20/30`; creation of `40` fails; original values unchanged; `3` live objects.

### P02 · `Pool.RecycledSlotChangesGeneration`

- **Prepare:** Capacity `1`; A=`7` with ID `(slot=0, generation=g)`; then B=`42`.
- **Run:** Destroy A, then create B.
- **Check:** B uses `slot=0` but its generation differs from `g` (for example `g+1`); B's ID differs from A's.

### P03 · `Pool.StaleHandleRejected`

- **Prepare:** Capacity `1`; A=`7`, destroyed; B=`42`, created afterward; old A ID retained.
- **Run:** Look up A and B.
- **Check:** `get(idA)` returns no object; `get(idB)` returns `42`, never `7`.

### P04 · `Pool.LiveAddressStable`

- **Prepare:** Capacity `3`; create A=`10`, record address `p`, then create B=`20` and C=`30`.
- **Run:** Look up A after creating B and C.
- **Check:** A's address is still exactly `p`, and the value is still `10`.

### P05 · `Pool.ClearInvalidatesHandles`

- **Prepare:** Capacity `2`; create A=`10`, B=`20`; retain IDs; later create C=`30`.
- **Run:** Clear the pool, try the old IDs, then create C.
- **Check:** No old ID is valid; pool is empty after `clear()`; C is created successfully and has value `30`.

## GP-T2 — Entity composition, inheritance and factory (6 tests)

Test abstract ownership and virtual dispatch without opening a graphics window. Use real Cyclone types to inspect composed colliders.

### GO01 · `Entities.FactoryCreatesDerivedTypes`

- **Prepare:** `SceneObjects` capacity `4`; call `createEntity<StandardProjectile>()`, then `createEntity<HeavyProjectile>()`.
- **Run:** Construct concrete entities through the C++20-constrained factory and access each as `GameEntity*`.
- **Check:** Two distinct valid `EntityId`s, two live entities, each non-null `GameEntity*`, each concrete collider linked to its own body; no cast is used.

### GO02 · `Entities.VirtualDrawDispatchWithoutRTTI`

- **Prepare:** A test-only `GameEntity` subclass implementing all abstract operations, with `draw() const` incrementing a counter initialized to `0`; access via `GameEntity&`.
- **Run:** Call `draw()` twice through the base reference, without opening a raylib window.
- **Check:** Counter equals exactly `2`; virtual dispatch uses the test subclass; no `dynamic_cast` and no actual graphics API call.

### GO03 · `Entities.ProjectileProfilesAreDistinct`

- **Prepare:** Two configured projectiles: standard profile mass `1 kg`, radius `0.28 m`; heavy profile mass `2 kg`, radius `0.36 m`.
- **Run:** Initialize both with their concrete configuration routines and query Cyclone body mass and `CollisionSphere::radius`.
- **Check:** Standard: `(mass=1, radius=0.28)`; heavy: `(mass=2, radius=0.36)` with tolerance `10⁻⁴`; both derive from `Projectile`, which composes `CollisionSphere` and can share sphere-drawing and launch logic.

### GO04 · `Entities.PillarCubeAndCageProfiles`

- **Prepare:** Construct `PillarBlock`, `CubeBlock`, and **one** `CageBlock` with centers `(0,1.5,0)`, `(0,3.5,0)`, `(3.6,0.65,0)`; cage dimensions `0.7×1.3×1.0 m`.
- **Run:** Configure each concrete derived type and query its Cyclone body and `CollisionBox`.
- **Check:** Pillar: mass `3 kg`, half-size `(0.5,1.5,0.5)`; cube: mass `1.5 kg`, half-size `(0.5,0.5,0.5)`; cage: inverse mass `0`, half-size `(0.35,0.65,0.5)`, exactly **one** box collider; all dimensions within `10⁻⁴ m`.

### GO05 · `Entities.StableColliderAddressAndSafeRemoval`

- **Prepare:** Create one `PillarBlock` with valid ID `a`, record body/collider addresses; then create 3 more objects while it is registered; finally unregister `a` and remove it.
- **Run:** Compare addresses before removal, then check lookup and registration after destruction.
- **Check:** Both recorded addresses remain identical while `a` is registered; `PhysicsWorld` has no registration for `a` after `unregisterObject(a)`; `SceneObjects::get(a)` returns no entity after removal.

### GO06 · `Entities.ConstrainedFactoryRejectsUnrelatedTypes`

- **Prepare:** Concrete `StandardProjectile` and unrelated `int`; C++20 concept/`requires` detection used in the test translation unit.
- **Run:** Check factory invocability at compile time with `static_assert`, and perform one valid creation at runtime.
- **Check:** `createEntity<StandardProjectile>()` is well-formed, `createEntity<int>()` is not; the test translation unit compiles without needing to intentionally fail the build.

## GP-T3 — Orbital camera and replaceable camera controller (6 tests)

Use fake input snapshots, no window. Unless a test specifies otherwise, use `1e-3 m` position and `1e-4` direction tolerances.

### C01 · `Camera.TargetAndRadiusRemainStable`

- **Prepare:** Target `T=(2,1,0)`; radius `r=10`; yaw `0°`, pitch `20°`, then yaw `90°`, pitch `30°`.
- **Run:** Recalculate the camera pose after changing the angles.
- **Check:** Initial position `(2, 4.4202, 9.3969)`; final `(10.6603, 6, 0)`; target remains `(2,1,0)`; distance to T is `10 ± 10⁻³ m`.

### C02 · `Camera.PitchAndZoomAreClamped`

- **Prepare:** Example limits: pitch `[10°,80°]`, distance `[4,25] m`; requests `−30°`, `2 m`, then `100°`, `40 m`.
- **Run:** Apply out-of-range settings.
- **Check:** First clamped state: `10°`, `4 m`; second: `80°`, `25 m`.

### C03 · `Camera.FullTurnReturnsToSamePose`

- **Prepare:** Target `(0,0,0)`; `r=10`; pitch `20°`; yaw initially `30°`, finally `390°`.
- **Run:** Complete one horizontal orbit.
- **Check:** Same position before and after, approximately `(4.6985, 3.4202, 8.1380)`; positional difference `< 10⁻⁴ m`.

### C04 · `Camera.RotationSpeedUsesDeltaTime`

- **Prepare:** Target `(0,0,0)`; distance `10 m`; initial yaw/pitch `0°`; pitch limits `[−80°,80°]`; speed `90°/s`; normalized horizontal input `+1`; (a) `dt=0.5 s`, (b) two updates of `dt=0.25 s`.
- **Run:** Apply identical orbit input for the same total duration.
- **Check:** In both cases final yaw `45° ±10⁻³°`, position `(7.0711,0,7.0711) m ±10⁻³ m`; identical camera poses for (a) and (b).

### C05 · `Camera.MouseDragIsFrameRateIndependent`

- **Prepare:** Target `(0,0,0)`, distance `10 m`, initial yaw/pitch `0°`; sensitivity `0.25°/pixel`, maximum speed `360°/s`; (a) `60` frames × `2 px` at `60 FPS`, (b) `30` frames × `4 px` at `30 FPS`.
- **Run:** Apply raw horizontal mouse deltas; do not multiply mouse pixel-to-angle conversion by `dt` a second time.
- **Check:** Both runs end at yaw `30° ±0.001°` and camera position `(5,0,8.6603) m ±0.001 m`; same target and no pitch change.

### CAM06 · `Camera.ManagerCanReplaceController`

- **Prepare:** CameraManager initially holds an `OrbitCamera`; `Launcher` azimuth `20°`, elevation `30°`; substitute a test `ICameraController` returning pose `(5,2,5)`, then call `getCamera()`.
- **Run:** Switch camera controllers through the interface without modifying the launcher.
- **Check:** Camera pose comes from the test controller at `(5,2,5)`; launcher direction remains `(0.8138,0.5000,0.2962)` within `10⁻⁴`; no cast or real `FreeCamera` implementation is required.

## GP-T4 — Launcher and shot kind (5 tests)

Launch parameters are independent of camera pose. Verify axis signs, clamping and projectile selection.

### L01 · `Launcher.ZeroAzimuthHasNoZComponent`

- **Prepare:** Azimuth `a=0°`; elevation `e=30°`.
- **Run:** Compute launch direction.
- **Check:** `d ≈ (0.8660, 0.5000, 0.0000)`; Z component `0 ± 10⁻⁴`.

### L02 · `Launcher.DirectionIsNormalized`

- **Prepare:** Case 1: `a=30°`, `e=30°`; case 2: `a=−30°`, `e=45°`.
- **Run:** Compute both directions and their lengths.
- **Check:** Case 1: `(0.7500, 0.5000, 0.4330)`; case 2: `(0.6124, 0.7071, −0.3536)`; both have `‖d‖ = 1 ± 10⁻⁴`.

### L03 · `Launcher.ParametersAreClamped`

- **Prepare:** Limits: azimuth `[−30°,30°]`, elevation `[5°,55°]`, speed `[8,22] m/s`; requested `(45°,−10°,30 m/s)`.
- **Run:** Set requested aim parameters.
- **Check:** Stored values: `(30°,5°,22 m/s)`.

### L04 · `Launcher.AimIsIndependentFromCamera`

- **Prepare:** Launcher `a=20°`, `e=30°`; camera yaw initially `0°`, then `120°`.
- **Run:** Recompute aim direction after rotating only the camera.
- **Check:** Both times `d ≈ (0.8138, 0.5000, 0.2962)`; per-component difference `< 10⁻⁴`.

### L05 · `Launcher.SelectsConcreteProjectileKind`

- **Prepare:** Start selected kind `Standard`, press selection key `2`, then `1`, with aim speed `16 m/s`.
- **Run:** Build `ShotSettings` after each key press and invoke the corresponding `SceneObjects` factory at launch.
- **Check:** After `2`, `ShotSettings.projectileKind=Heavy` and a `HeavyProjectile` is created; after `1`, kind `Standard` and `StandardProjectile` is created; both receive the same world-space aim and `16 m/s` speed.

## GP-T5 — Real Cyclone projectile (5 tests)

Use the real Cyclone integrator, detectors and resolver for shot behavior; rendering must not affect the physical step count.

### PR01 · `Projectile.OnlyOneShotPerKeyPress`

- **Prepare:** `10` updates with the key held; a single initial key-pressed event.
- **Run:** Process shot commands across the hold.
- **Check:** `1` projectile created and `1` round consumed, not `10`; no second shot without a new allowed press edge.

### PR02 · `Projectile.InitialVelocityMatchesAim`

- **Prepare:** Start `(-8,1,0) m`; azimuth `0°`, elevation `30°`; launch speed `20 m/s`.
- **Run:** Create the Cyclone body and immediately read its initial velocity.
- **Check:** `v₀≈(17.3205, 10.0000, 0.0000) m/s`; per-component error `< 10⁻³ m/s`.

### PR03 · `Projectile.FallsUnderGravity`

- **Prepare:** Sphere with no ground, initial position `(0,20,0) m`; `v₀=(0,0,0)`; `g=(0,−9.81,0)`; linear damping `1`; `60` steps of `1/60 s`.
- **Run:** Integrate `1.00 s` of free fall with Cyclone.
- **Check:** After 60 steps, `vᵧ≈−9.81 m/s` (`±0.05`), `y≈15.01 m` (`±0.10`); damping `1`, sleeping disabled; no manual physics integration.

### PR04 · `Projectile.DoesNotCrossGround`

- **Prepare:** Sphere radius `0.50 m`, initial center `(0,3,0) m`, zero velocity, ground `y=0`, `180` steps of `1/60 s`.
- **Run:** Simulate fall, contact detection, and contact resolution.
- **Check:** At least `1` sphere–ground contact; by the final step the center remains near resting height (target `y≈0.5 m`, initial tolerance `±0.1 m`), with no complete passage through the ground plane. Configure the resolver to satisfy this invariant.

### PR05 · `Projectile.IsIndependentOfRenderFPS`

- **Prepare:** Same body at `(0,50,0) m`, `v₀=(10,0,0) m/s`, `g=−9.81 m/s²`, damping `1`, step `1/60 s`; render `60` frames at `30 FPS` and `288` frames at `144 FPS` (`2 s` in both cases).
- **Run:** Execute exactly `120` physics steps (`2.00 s`) for each run.
- **Check:** `120` physics steps in both runs; same state, position difference `≤10⁻⁴ m` and velocity difference `≤10⁻⁴ m/s`; shared example `x≈20 m`, `vᵧ≈−19.62 m/s`.

## GP-T6 — Ghost trajectory via Cyclone (4 tests)

The ghost uses an isolated Cyclone body and snapshots. Verify first real contact and absolutely no mutation of live physics.

### T01 · `Trajectory.StartsAtLauncher`

- **Prepare:** Start `p₀=(−8,1,0) m`; velocity `(10,5,0) m/s`; radius `0.20 m`; `dt=1/60 s`.
- **Run:** Build prediction and read the initial point, before integration.
- **Check:** First point exactly `(-8,1,0) m` (tolerance `10⁻⁴ m`); no change to the real world.

### T02 · `Trajectory.StopsAtFirstGroundContact`

- **Prepare:** Ghost center `(0,1.51,0) m`, radius `0.50 m`, velocity `(0,−1,0) m/s`, zero acceleration, damping `1`; ground `y=0`; `dt=1/60 s`; `120`-step limit.
- **Run:** Integrate with Cyclone and test sphere against the ground after each step until the first contact.
- **Check:** First contact **on step 61**; ghost center `y≈0.4933 m` (`±0.005 m`); contact marker near `y≈0 m`; no step 62 and no later trajectory point.

### T03 · `Trajectory.DoesNotModifyPhysicsState`

- **Prepare:** Real witness body at `(-8,1,0) m`, velocity `(12,5,0) m/s`; request `20` ghost steps at `1/60 s` with no obstacles.
- **Run:** Generate isolated prediction, then read back real body states and body count in `PhysicsWorld`.
- **Check:** `21` positions (start + 20 steps); real body still at `(-8,1,0) m`, velocity `(12,5,0) m/s` (`±10⁻⁴`); real body count unchanged; no real contacts added.

### T04 · `Trajectory.StopsAtNearestObstacle`

- **Prepare:** Ghost center `(−2.03,1,0) m`, radius `0.20 m`, velocity `(3,0,0) m/s`, gravity `0`, damping `1`; fixed box centered at `(0,1,0) m`, half-sizes `(0.25,0.25,0.25) m`; ground `y=0`; `dt=1/60 s`.
- **Run:** Predict contacts against **both the box and the ground**.
- **Check:** First hit is **the box**, on step `32`; ghost center `x≈−0.43 m` (`±0.005 m`); at most `33` positions (start + 32 steps); no ground contact and no trajectory segment past the box.

## GP-T7 — Pillar and top cube (3 tests)

Shared dynamic-box fixture: pillar `1×3×1 m`, `3 kg` at `(0,1.5,0)`; cube `1×1×1 m`, `1.5 kg` at `(0,3.5,0)`. They touch at `y=3`, without penetration. Set `dt=1/60 s`, gravity `(0,-9.81,0)`, friction `0.5`, restitution `0`, damping `0.98/0.85`, solver `100/100`; use bounded, not exact, final poses.

### CO01 · `Tower.RemainsStandingAtRest`

- **Prepare:** Shared reference pillar and top cube, zero initial speeds, sleeping enabled, **780 steps** at `1/60 s`, no projectile.
- **Run:** Simulate `13 s` with real Cyclone and check all rigid-body poses; physical box–ground and box–box contacts must be generated.
- **Check:** Both bodies remain in vertical order; maximum horizontal displacement of either body `≤0.15 m`, maximum pillar tilt `≤5°`, and each center's vertical deviation `≤0.10 m`. Actual reference run: `0.105 m` maximum horizontal drift, `0.50°` maximum pillar tilt, and `0.019 m` maximum vertical deviation.

### CO02 · `Tower.BoxesCollideWithEachOther`

- **Prepare:** Shared pillar at `(0,1.5,0)`; start the top cube instead at `(0,4.5,0)` with velocity `(0,−2,0) m/s`; simulate `120` steps at `1/60 s`.
- **Run:** Drop the cube onto the standing pillar and resolve the box–box contacts via Cyclone.
- **Check:** At least `1` real pillar–cube contact; the cube remains above the pillar (cube center Y greater than pillar center Y), and the center separation along Y stays `≥1.8 m` after resolution. The real test obtained `103` contacts and minimum Y separation `≈1.953 m`.

### CO03 · `Tower.OffCenterHitProducesRotation`

- **Prepare:** Shared pillar and top cube initially upright; projectile sphere radius `0.28 m`, mass `1 kg`, center `(−8,1.1,0)`, initial velocity `(16,1.5,0) m/s`; simulate up to `600` steps.
- **Run:** Fire into the lower part of the pillar and simulate the genuine rigid-body interactions.
- **Check:** At least one sphere–pillar contact; the pillar tilts by `≥15°` from its initial orientation, and the top cube loses its support and reaches center Y `≤1.0 m` within the run. In the audited reference the pillar rotated by about `90°` and the cube reached ground level.

## GP-T8 — Single solid CageBlock and direct destruction (6 tests)

Shared cage: one fixed `CollisionBox` at `(3.6,0.65,0)`, half-size `(0.35,0.65,0.5)`, inverse mass and inertia zero. `DamageSystem` breaks on prepared `Enter ≥3.0 m/s`, **after** solving; no physical bars, bird or fracture.

### CA01 · `Cage.BelowThresholdDoesNotBreak`

- **Prepare:** One intact `CageBlock` ID `7`, threshold `3.0 m/s`; projectile–cage `Enter` speed `2.9 m/s`.
- **Run:** Dispatch the event to `DamageSystem`.
- **Check:** Cage still alive and drawable, `0` queued removals, `0` destruction increments.

### CA02 · `Cage.AtOrAboveThresholdBreaks`

- **Prepare:** One fixed `CageBlock` ID `7`, threshold `3.0 m/s`; two `Enter` events of `3.0` and `5.0 m/s`.
- **Run:** Publish both and inspect pending removals.
- **Check:** After first event: exactly `1` `EntityId` queued, `1` break and `1` score increment; second event causes **no additional** break or removal.

### CA03 · `Cage.RestingContactIsNotDamage`

- **Prepare:** Cage ID `7` and cube ID `2`; 120 steps of contact at `0.02 m/s`; impact event threshold `1.0 m/s`.
- **Run:** Send snapshots through generator and dispatcher.
- **Check:** `0` emitted gameplay events, cage intact and `0` removals, score `0`.

### CA04 · `Cage.SameRuleForProjectileAndCube`

- **Prepare:** Two independent intact one-box cages (IDs `7` and `8`) with threshold `3.0 m/s`; projectile–cage 7 `Enter` at `4.0 m/s`, cube–cage 8 `Enter` at `4.0 m/s`.
- **Run:** Dispatch both events through the **same** `DamageSystem`.
- **Check:** Exactly `1` removal per cage, `2` cage breaks in total, with no branch based on projectile or cube concrete type.

### CA05 · `Cage.DestructionIsDeferred`

- **Prepare:** One `CageBlock` ID `7`, one `RigidBody` and one `CollisionBox`; `Enter` speed `4.0 m/s` during a contact step.
- **Run:** Mark removal after the observed contact; resolve; unregister; then destroy from pool.
- **Check:** During resolution body and collider addresses remain valid; after cleanup world has `0` cage registrations, `get(7)` returns `nullptr`, and next physics step uses no cage pointer.

### CA06 · `Cage.HasOneColliderAndRemainsQuietAtRest`

- **Prepare:** One `CageBlock` at `(3.6,0.65,0) m`, half-size `(0.35,0.65,0.5) m`, inverse mass `0`, a fixed ground plane at `y=0`, and **600** physics steps of `1/60 s` with no dynamic objects.
- **Run:** Register the single cage box, step the world, then read registration/physical-contact/event counts. Skip fixed–ground collision generation.
- **Check:** Exactly **1** cage registration and **1** `CollisionBox`; **0** cage–ground contact pairs and **0** `Enter/Exit` events over all 600 steps; cage pose and alive state unchanged.

## GP-T9 — Indirect chain reaction (3 tests)

Reuse GP-T7 tower and GP-T8 cage. Standard sphere: `1 kg`, radius `0.28 m`, origin `(-8,1,0)`, speed `16 m/s`, yaw `0°`, elevation `26°`. Audit reference ≈`5.56 m/s` cube–cage, zero physical projectile–cage; this is **not** an exact solver endpoint assertion.

### CH01 · `Chain.ProjectileNeverTouchesCage`

- **Prepare:** Pillar `(0,1.5,0)`, cube `(0,3.5,0)`, fixed cage box center `(3.6,0.65,0)`, half-size `(0.35,0.65,0.5)`; sphere from `(-8,1,0)`, radius `0.28`, speed `16 m/s`, azimuth `0°`, elevation `26°`; max `360` steps at `1/60 s`.
- **Run:** Run the actual Cyclone world and inspect **physical** pairs before event filtering.
- **Check:** At least `1` projectile–pillar contact, **0 projectile–cage contacts**, and actual movement of the dynamic structure.

### CH02 · `Chain.FallingCubeBreaksCage`

- **Prepare:** The CH01 world; cage break threshold `3.0 m/s`; max `360` steps.
- **Run:** Simulate, emit `CollisionEnter`, resolve, then unregister and delete the cage.
- **Check:** At least `1` cube–cage `Enter` with closing speed **≥3.0 m/s**; **1** cage break, **1** deferred cage removal and **0 projectile–cage** physical contacts. Audited example: speed `≈5.56 m/s`, zero-based step `73`, cube descending (`vᵧ≈−1.6 m/s`).

### CH03 · `Chain.RepeatableAfterReset`

- **Prepare:** One pillar, one cube, one cage (**3** pre-shot scene entities, excluding ground), and one projectile per run; `5` resets with the CH01 shot.
- **Run:** Replay the complete physics/gameplay sequence five times with listener and pair-state reset.
- **Check:** Each reset starts with exactly `3` scene entities and an intact cage; each replay yields exactly `1` cage break and `0` projectile–cage contacts; old cage IDs rejected after removal.

## GP-T10 — State machine / lifecycle / reset (9 tests)

State lifecycle calls are observable and happen once; reset must discard physics and listener history safely.

### GS01 · `GameState.StartToPlaying`

- **Prepare:** Initial state `Start`, `3` rounds, `1` cage; commands `Space`, then `Enter`.
- **Run:** Apply commands in order.
- **Check:** After `Space`: state `Start`, `0` shots; after `Enter`: state `Playing`, `3` rounds, `1` intact cage.

### GS02 · `GameState.WinShowsGameOver`

- **Prepare:** State `Playing`; `1` cage remaining; one `CageDestroyedEvent`, then a duplicate.
- **Run:** Apply both notifications.
- **Check:** After first: state `GameOver`, `GameResult=Win`, `0` cages remaining; duplicate changes nothing, score credited only **once**.

### GS03 · `GameState.LoseWaitsForSettling`

- **Prepare:** `Playing`, `0` rounds, `1` cage; projectile initially active; later inactive with continuous rest lasting `1.5 s`, configured minimum `1.0 s`.
- **Run:** Inspect state while flying and after settling.
- **Check:** While flying: `Playing`; before `1.0 s` of rest: `Playing`; after `1.5 s`: `GameOver`, `GameResult=Lose`.

### GS04 · `GameState.RestartResetsEverything`

- **Prepare:** `GameOver(Win)`, score `100`, `0` cages, pool holding old scenario objects; `R`, then `Enter`.
- **Run:** Return to `Start`, then launch a new game.
- **Check:** After `R`: `Start`, score `0`, `3` rounds, `1` cage; old `EntityId`s invalid, old collision pairs/subscriptions removed; after `Enter`: `Playing`.

### GS05 · `GameState.PlayingOnlyAcceptsFire`

- **Prepare:** `Start`, then `Playing` with `3` rounds; three updates with `Space` held but only one press edge; switch to `GameOver` and send `Space` again.
- **Run:** Inject commands without a window.
- **Check:** In `Start`: `0` shots; in `Playing`: `1` projectile, `2` rounds; in `GameOver`: no extra shot, still `2` rounds.

### GS06 · `GameState.RestartRestoresSingleListenerSubscription`

- **Prepare:** One `DamageSystem` listener and one `CageBlock`; perform `3` game resets, then publish one `Enter` at `4.0 m/s` to the current cage.
- **Run:** Unsubscribe previous listeners, clear collision-pair history, rebuild current world, and subscribe once.
- **Check:** Exactly `1` callback to the current listener and `1` cage destruction; `0` callbacks to old listeners, `0` duplicate subscriptions.

### GS07 · `GameState.EnterExitCalledExactlyOnceInOrder`

- **Prepare:** Start-up in `Start`; commands `Enter`, then one `CageDestroyedEvent` with outcome `Win`, then `R`. Record every state lifecycle callback in a list.
- **Run:** Start the machine, apply the commands and collect the log.
- **Check:** Exactly `[Start.enter, Start.exit, Playing.enter, Playing.exit, GameOver.enter, GameOver.exit, Start.enter]` in this order; **no extra calls**.

### GS08 · `GameState.NoOpTransitionHasNoLifecycleCallbacks`

- **Prepare:** Machine already in `Playing`; attempt `transition(Playing)` three times and run `120` update frames; lifecycle call counters reset immediately before these operations.
- **Run:** Repeat same-state transitions and updates.
- **Check:** `Playing.onEnter = 0`, `Playing.onExit = 0`, `Start.onEnter = 0`, `GameOver.onEnter = 0`; current state still `Playing`.

### GS09 · `GameState.SettlingTimeoutPreventsSoftlock`

- **Prepare:** `Playing`, `0` rounds remaining, `1` intact cage, no projectile in flight; world movement never stays below the quiet threshold due to a jittering box; maximum settling timeout `5.0 s`; fixed step `1/60 s`.
- **Run:** Advance `300` fixed steps while reporting nonzero residual box movement.
- **Check:** At or before the end of step `300`, state is `GameOver`, result `Lose`; no indefinite `Playing` loop and the transition lifecycle executes once.

## GP-T11 — Audio commands, no audio device (3 tests)

Test commands only. Real raylib sound playback is checked manually, not in GoogleTest.

### AU01 · `Audio.ShotSoundIsTriggeredOnce`

- **Prepare:** One accepted shot; key-held event for `10` updates; audio output replaced with a command counter.
- **Run:** Pass gameplay commands to the audio feedback system.
- **Check:** `1` `PlayShot` command, not `10`; actual raylib playback verified manually.

### AU02 · `Audio.CageBreakSoundIsUnique`

- **Prepare:** One `CageBlock` ID `7`, threshold `3.0 m/s`, two significant `Enter` events of `4` and `5 m/s` for the same cage.
- **Run:** Dispatch events and inspect audio command sink.
- **Check:** Exactly `1` `PlayCageBreak` command and `1` cage removal request; no replay after the first event.

### AU03 · `Audio.GameOverSoundOnEntryOnly`

- **Prepare:** State changes `Playing → GameOver(Win)`; then `120` updates while in `GameOver`.
- **Run:** Observe the `GameOverState::onEnter` audio request and later per-frame updates.
- **Check:** `1` `PlayWin` command and no repetitions across the following `120` updates.

## Manual acceptance

These are visual/audio scenarios to check in the executable; they do not add cases to the 55 GoogleTests.

- Orbit around a stationary target using right-mouse drag and wheel zoom for 30 seconds; verify smooth motion at multiple frame rates.
- Launch one real spherical projectile for one Space press; let Cyclone generate and resolve its ground contact.
- Visualize the Cyclone ghost without moving any active physics object; stop at first contact.
- Keep the pillar + cube at rest for 10 seconds; verify contact stability and absence of significant gameplay events.
- Break the **single** CageBlock by a direct hit, then restart and break it via the falling cube, without a projectile–cage contact.
- Win/lose/restart: screens, one-shot sounds, no stale entity ID or double subscriber.
