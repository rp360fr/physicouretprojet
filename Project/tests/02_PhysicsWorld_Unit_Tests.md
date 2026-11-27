# Siege Lab — PhysicsWorld Unit Tests (25 cases)

## Test rules

Use a fresh fixture for every case. Avoid a raylib window or audio device: test `draw()` dispatch with a headless subclass and sound requests with a command sink. Verify graphics and listening manually in `SiegeLabGame`.

- Use `dt=1/60 s` unless specified; `+Y` up and `+X` forward.
- Assert IDs, counts and state transitions exactly; use the tolerance stated per floating-point assertion.
- Link **the real teacher-pinned Cyclone** for tests that claim solver or detector behavior.
- No `dynamic_cast`, `std::variant` or `std::visit`; clear registrations, listeners and pair history between tests.

## Event reference settings

Use these thresholds and IDs in every event test. A resting physical contact is **not** automatically a gameplay impact.

| Contract | Required value |
|---|---|
| Fixed tick | `1/60 s`, unless the case says otherwise |
| Significant episode start | prepared closing speed **≥1.0 m/s**; one `CollisionEnter` per unordered entity pair |
| Quiet episode end | 3 **consecutive** checked contact steps with closing speed ≤0.2 m/s; one `CollisionExit` |
| Missing-contact episode end | 2 **consecutive fully checked** steps without that pair; one `CollisionExit` |
| Subthreshold-only contact | Neither Enter nor Exit |
| Multiple contact points | Keep **maximum** prepared closing speed, with that point/normal; one pair event |
| Pair identity | Canonical two `EntityId`s (slot + generation); fixed ground uses a separate `WorldGround` ID |
| Incomplete buffer coverage | **Never** count an unchecked pair as missing; EV19 regression is mandatory |
| Solver observation | After `prepareContacts`, before position/velocity correction; copy values, never store `Contact*` |
| Fixed cage–ground | No fixed–fixed detection; fixed cage is **one CollisionBox** |

### Real-physics fixture

Use the complete supplied Cyclone, not a mock. For EV18, the dynamic 3 kg pillar (1×3×1 m) at (0,1.5,0) supports the 1.5 kg cube (1×1×1 m) at (0,3.5,0), friction 0.5, restitution 0, gravity -9.81 m/s², damping 0.98/0.85, 100/100 resolver iterations. Simulate 180 settling steps, reset the gameplay event history, then observe 600 steps. **Physical contacts must remain present while gameplay Enter/Exit counts remain zero.**

## Group map

`PW01–PW05` validate M6. `PW06` is a **M7** regression after the permitted Cyclone buffer fix; `EV01–EV19` also belong to M7.

| Group | IDs | Count | PhysicsWorld milestone |
|---|---|---:|---|
| PhysicsWorld integration | PW01–PW05 | 5 | PW-M6 |
| Contact-buffer patch regression | PW06 | 1 | PW-M7 (requires `boxAndHalfSpace` fix) |
| Cyclone patch, snapshots and events | EV01–EV19 | 19 | PW-M7 |
| **TOTAL** | | **25** | |

## PW-T1 — PhysicsWorld registration, stepping and capacity (6 tests)

Verify non-owning registration and one Cyclone step. `PW06` becomes a passing regression only after the M7 capacity patch.

### PW01 · `PhysicsWorld.RegisterBorrowsObject`

- **Prepare:** Pool with `1` object A and a stable `RigidBody*`; world initially empty.
- **Run:** Register A, then unregister it without deleting A from the pool.
- **Check:** Registration count goes `0 → 1 → 0`; `pool.get(A)` remains valid and the body's address does not change.

### PW02 · `PhysicsWorld.StepUsesCyclone`

- **Prepare:** Two identical spheres without ground, initially at `(0,20,0) m`, zero speed, gravity `(0,−9.81,0)`, damping `1`, sleeping disabled; `60` steps at `1/60 s`.
- **Run:** Simulate first body through `PhysicsWorld::step` and second via the equivalent sequence of direct Cyclone calls.
- **Check:** After 1 second, position and velocity differences between the two bodies are `<10⁻⁴`; vertical speed near `−9.81 m/s`.

### PW03 · `PhysicsWorld.RegisteredColliderIsUsed`

- **Prepare:** Sphere radius `0.5 m`, center `(0,0.45,0) m`, ground at `y=0`, zero gravity; one registerable sphere.
- **Run:** Register sphere and ground, detect contacts; unregister sphere and repeat detection after resetting world state.
- **Check:** Registered sphere: `≥1` sphere–ground contact; unregistered sphere: `0` contacts involving it.

### PW04 · `PhysicsWorld.NoDoubleIntegrationAfterMerge`

- **Prepare:** Dynamic body initially `(0,10,0)`, velocity `(1,0,0) m/s`, gravity `0`, damping `1`, `60` steps at `1/60 s`.
- **Run:** After replacing the old local loop with `PhysicsWorld::step`, simulate `1 s`.
- **Check:** `x≈1.000 m` (`±0.001 m`), never `2 m`; exactly `60` integrations, not `120`.

### PW05 · `PhysicsWorld.UnregisterBeforeDeletion`

- **Prepare:** Pool contains body A and its collider; world has A registered; deletion requested after a contact step.
- **Run:** Finish solving, unregister A, destroy A in the pool, then run another step.
- **Check:** No registration for A, `get(A)==nullptr`, `0` contacts containing A during the next step, and no use of invalidated pointers or exceptions.

### PW06 · `PhysicsWorld.ContactBufferNeverOverflows`

- **Prepare:** `CollisionData` has a valid array of capacity `4` with canaries immediately after it; a test box lies fully below a ground plane so that up to `8` vertex contacts are possible.
- **Run:** Generate box–ground contacts with the allowed Cyclone bookkeeping correction and bounded `PhysicsWorld` dispatch; attempt another detector call with no room left.
- **Check:** Exactly `4` contacts recorded; `contactCount=4`, `contactsLeft=0`, no canary overwritten, and the second detector is skipped. The test fails on the unpatched early-return bookkeeping path.

## PW-T2 — Collision snapshots, patch, Enter/Exit and listeners (19 tests)

Use synthetic snapshots for deterministic episode thresholds, and real Cyclone contacts for prepared-speed, rotation and resting-stack tests.

### EV01 · `CollisionEvent.ContactProducesEnter`

- **Prepare:** Step 12; pair A=`(1,1)`, B=`(2,1)`; point `(1,1,0)`; normal `(−1,0,0)`; prepared Cyclone speed `4.0 m/s`.
- **Run:** Generate and publish step 12.
- **Check:** Exactly **1 `Enter`**, `impactSpeed=4.0±0.001 m/s`, with matching IDs, point, normal, and step index.

### EV02 · `CollisionEvent.NoContactProducesNoEvent`

- **Prepare:** Steps 1–10, no pairs present and no previous events.
- **Run:** Process 10 empty steps.
- **Check:** **0 `Enter`, 0 `Exit`**; no active pair.

### EV03 · `CollisionEvent.FourContactsGiveOnePair`

- **Prepare:** Step 8; 4 A–ground contacts with speeds `(1,2,4,3) m/s`; the maximum `4 m/s` occurs at point `(2,1,0)`.
- **Run:** Aggregate step 8.
- **Check:** **Just 1 `Enter`**, `impactSpeed=4 m/s`, point `(2,1,0)`; not four notifications.

### EV04 · `CollisionEvent.EnterThenExitOnly`

- **Prepare:** Same A–B pair: step 1 speed `4.0`; steps 2–4 speed `0.05`; steps 5–6 no contact.
- **Run:** Process all 6 steps.
- **Check:** **`Enter` on step 1 and `Exit` on step 4**, nothing later; **2 total notifications**, no periodic events on steps 2, 3, 5, or 6.

### EV05 · `CollisionEvent.PairOrderingIsStable`

- **Prepare:** Step 1: `(A,B)` speed `4 m/s`, normal `(−1,0,0)`; step 2: `(B,A)` speed `4 m/s`, opposite normal.
- **Run:** Process both steps.
- **Check:** **1 `Enter` on step 1**, **0 events on step 2**; same canonical pair key and no duplicate.

### EV06 · `CollisionEvent.GroundHasNoBodyPointer`

- **Prepare:** A=`(4,2)` against `CollisionPlane`, `body[1]=nullptr`, prepared speed `3 m/s`, step 1.
- **Run:** Capture and convert the contact.
- **Check:** **1 `Enter` for A–`WorldGround`**, speed `3 m/s`; no null-pointer dereference.

### EV07 · `CollisionEvent.ResetInvalidatesPreviousPairs`

- **Prepare:** Old step 1: A=`(1,1)`/B=`(2,1)`, speed `4`; reset; new step 1: A=`(1,2)`/B=`(2,2)`, speed `4`.
- **Run:** Emit, reset, and start again.
- **Check:** **1 `Enter` in each game session**, **0 leftover `Exit`** and no reused stale ID.

### EV08 · `CollisionEvent.RealCycloneContactIsPublished`

- **Prepare:** Two Cyclone spheres, radius `0.5 m`, centers `(0,1,0)` and `(0,1,0.8)`, velocities `(0,0,2)` and `(0,0,−1) m/s`, `dt=1/60 s`.
- **Run:** Run `sphereAndSphere`, solver observer, and event generation.
- **Check:** At least `1` physical contact and exactly `1 Enter` for the pair, `impactSpeed≈3 m/s` (`±0.01`); no pointer into Cyclone's temporary buffer survives resolution.

### EV09 · `CollisionEvent.DispatcherNotifiesSubscribers`

- **Prepare:** Two counting listeners initially `(0,0)`; publish `Enter`, unsubscribe second listener, then publish `Exit`.
- **Run:** Dispatch both copied events.
- **Check:** After `Enter`: `(1,1)`; after `Exit`: `(2,1)`; no callback to the unsubscribed listener.

### EV10 · `CyclonePatch.ClosingSpeedIsExposed`

- **Prepare:** Prepared contact: body A approaches stationary B at `4 m/s` along the normal; no rotation or acceleration; `dt=1/60 s`.
- **Run:** Invoke the solver with the new observer and read the accessor from its callback.
- **Check:** `getClosingSpeed() = 4.000 ± 0.001 m/s`; observer called `1` time without changing contacts.

### EV11 · `CyclonePatch.ContactPointRotationIsIncluded`

- **Prepare:** Body A stationary in translation, angular velocity `(0,0,2) rad/s`, center `(0,0,0)`, contact point `(0,1,0)`, normal `(1,0,0)`; fixed B; `dt=1/60 s`.
- **Run:** Prepare a real contact with the patched resolver and read its accessor in the callback.
- **Check:** `getClosingSpeed()≈2.0 m/s` (`±0.01`) despite zero linear velocity at the center; Cyclone's prepared velocity includes rotation.

### EV12 · `CyclonePatch.ObserverRunsBeforeResolution`

- **Prepare:** Bodies approaching at `4 m/s`, restitution `0`, contact with positive penetration, followed by a resolver call; a second call with `0` contacts.
- **Run:** Check operation ordering and callback counts.
- **Check:** For nonempty input, callback called exactly `1` time **between** preparation and correction; observed value `4±0.001 m/s` before impulses. For `0` contacts, callback called `0` times.

### EV13 · `CollisionEvent.RestingStackIsSilent`

- **Prepare:** Synthetic snapshots of four persistent, quiet contact pairs (maximum closing speed `0.05 m/s`), repeated for **600 steps**.
- **Run:** Process snapshots while contacts persist.
- **Check:** **0 `Enter`, 0 `Exit`** over 600 steps; only internal contact state may change.

### EV14 · `CollisionEvent.SubthresholdContactNeverExits`

- **Prepare:** A–B speed `0.99 m/s` on steps 1–3; no contact on steps 4–5.
- **Run:** Process five steps.
- **Check:** **0 events**: no below-threshold `Enter` and no `Exit` for a pair that never entered.

### EV15 · `CollisionEvent.ReimpactWhileStillTouching`

- **Prepare:** Same pair: step 1 speed `4`; steps 2–4 `0.05`; step 5 `0.05`; step 6 `5`; steps 7–8 no contact.
- **Run:** Process all eight steps without separation between steps 1 and 6.
- **Check:** **Exactly 4 events:** `Enter(1)`, `Exit(4)`, `Enter(6)`, `Exit(8)`; second impact recognized.

### EV16 · `CollisionEvent.OneMissingStepDoesNotExit`

- **Prepare:** Pair: step 1 speed `4`; step 2 empty; step 3 contact `0.05`; steps 4–5 empty.
- **Run:** Process five steps.
- **Check:** **`Enter` on step 1 and `Exit` on step 5** only; no `Exit` on step 2 and no new `Enter` on step 3.

### EV17 · `CollisionEvent.IndependentPairs`

- **Prepare:** Same step: A–B `4 m/s`, B–ground `0.1 m/s`, A–C `2 m/s`.
- **Run:** Aggregate and dispatch.
- **Check:** **2 `Enter`** events (A–B, A–C), **0** B–ground events, even though contacts share an entity.

### EV18 · `CollisionEvent.RealRestingTowerDoesNotSpam`

- **Prepare:** Real Cyclone world: the calibrated **pillar + top cube** fixture from CO01, active gravity, `1/60 s` step; settle for `180` initial steps; event threshold `1 m/s`; observe for **600 additional steps**.
- **Run:** Run real simulation and dispatch captured contacts, resetting event generation before observation begins.
- **Check:** After calibrating the resting world: `0 Enter`, `0 Exit` during `600` steps, while pillar–ground and pillar–cube contacts are still generated. Test the real simulation, not fabricated contact data.

### EV19 · `CollisionEvent.IncompleteStepDoesNotCauseFalseExit`

- **Prepare:** Pair A–B: step 1 has a fully checked contact at `4 m/s`; steps 2–3 have no A–B snapshot **because a contact buffer overflow leaves that pair unchecked**; step 4 is fully checked and reports A–B at `0.05 m/s`.
- **Run:** Generate events while marking steps 2–3 as incomplete, then process step 4.
- **Check:** Exactly **1 `Enter` on step 1 and 0 `Exit` through step 4**; incomplete steps cannot count as absence.

## Integration checks

These review checks complement the 25 automated contracts. They are not counted as extra GoogleTest cases.

- A `CollisionPlane` is a separate geometry type and has **no owned rigid body**; ground-only contacts must not dereference `body[1]`.
- As `CollisionPrimitive` has **no virtual destructor**, the world cannot own or delete concrete colliders through its base pointer. Every source entity retains ownership, and its concrete collider stays alive until deregistration.
- A `CollisionBox` registered for the fixed CageBlock has inverse mass 0 and zero inverse inertia. It receives dynamic impacts but never fixed–fixed ground contacts and never undergoes integration.
- The `ContactResolver` getter is valid only after preparation. The observer makes independent copies and cannot run user `DamageSystem` callbacks before solving completes.
- After overflow, a pair might not have been examined; never fabricate its Exit. On `unregisterObject`, purge event history for every pair containing the removed handle.
- Implement both ordinary Debug and Release builds on the pinned Windows toolchain; sanitizer checks on a supported compiler are strongly encouraged. These are additional checks and do **not** change the 25 named cases.
