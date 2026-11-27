# Siege Lab — Explosion Unit Tests (15 cases)

## Common test requirements

Implement the **15 cases EX01–EX15** using the genuine supplied Cyclone library. Each case starts with a fresh fixture. Tests are headless: no raylib window and no audio device. Do not substitute mocks for Cyclone rigid bodies, generators, or contact resolution.

For isolated force tests (EX02–EX13), unless a case overrides a value:

- Fixed step: **1/60 s**; world axes: **+X forward**, **+Y upward**.
- Detonation: **(0, 0, 0)**; dynamic-body mass: **1 kg**; initial linear and angular velocities: **zero**.
- Initialize the `RigidBody` fully: finite positive inertia, zero acceleration, damping **1**, sleeping disabled, awake state set explicitly, derived data calculated, and force accumulators cleared.
- Disable gravity and contact solving. Apply the force generator, then call real Cyclone `RigidBody::integrate()` **exactly once** per evaluation.
- Measure the explosion's force contribution from the before/after velocity change of the same step, not from private accumulators. Subtract the body's initial velocity when it is nonzero.
- Absolute tolerance: **0.001 N** for inferred force components, **0.001 m/s** for velocity changes, **0.000001 s** for explosion age. All results must be finite.
- Set all explosion parameters explicitly for each fixture. Disable unused phases with a duration of **0 s**. Keep any registered bodies and generators alive until they have been unregistered.
- The test code may choose its own helper or time-control method names, provided they implement the behavior stated in the assignment.

## EX01 — `Explosion.ConstructibleAndDeterministic`

- **Prepare:** Create two independent `cyclone::Explosion` objects using their default constructors. Add a compile-time check that `Explosion` is not abstract.
- **Run:** Inspect every public numeric parameter and detonation coordinate. Read both explosion ages. Advance the first instance by **0.25 s** and the second by **0.40 s**; then reset only the first clock.
- **Check:** Construction and linking succeed. Every parameter and coordinate is finite. The default radii, durations, and peak forces satisfy the assignment's validity rules. Both ages initially equal **0 s**. After the operations, the first age is **0 s** and the second remains **0.40 s**, within the common age tolerance.

## EX02 — `Explosion.ImplosionPullsTowardCenter`

- **Prepare:** Configure implosion duration **0.5 s**, minimum radius **0.2 m**, maximum radius **3 m**, and force **12 N**. Disable shock and convection. Test separate bodies at **(1, 0, 0)** and **(-1, 0, 0)** at explosion age **0 s**. Add one translated-detonation case with detonation **(10, 0, 0)** and body **(11, 0, 0)**.
- **Run:** For each independent case, evaluate the force and integrate the body once.
- **Check:** The inferred X forces are **-12 N**, **+12 N**, and **-12 N**, respectively. Y and Z force components are **0 N**. All results meet the common force tolerance. The bodies accelerate toward their respective detonation positions.

## EX03 — `Explosion.ImplosionRespectsRadiusTimeAndCenter`

- **Prepare:** Use the implosion configuration from EX02 with detonation **(0, 0, 0)**. At explosion age **0 s**, independently test bodies at **(0.1, 0, 0)**, **(3.1, 0, 0)**, **(0, 0, 0)**, **(0.2, 0, 0)** and **(3, 0, 0)**. Also test a body at **(1, 0, 0)** at age **0.5 s**.
- **Run:** Evaluate and integrate each independent body once.
- **Check:** The inferred X forces, in the listed order, are **0 N**, **0 N**, **0 N**, **-12 N**, **-12 N**, and **0 N**. Y and Z forces are **0 N**. Every resulting velocity is finite. The inner and outer radius endpoints are included; the end-time boundary is excluded.

## EX04 — `Explosion.ShockFrontTravels`

- **Prepare:** Disable implosion and convection. Configure shock duration **1 s**, propagation speed **10 m/s**, thickness **1 m**, and peak force **100 N**. Use the assignment's reference taper setting. Create independent stationary bodies at **(2.5, 0, 0)** at ages **0 s** and **0.2 s**, and at **(3.5, 0, 0)** at age **0.3 s**.
- **Run:** Evaluate and integrate once for each case.
- **Check:** The inferred X forces are **0 N**, **+80 N**, and **+70 N**, respectively, within tolerance. Y and Z forces are **0 N**.

## EX05 — `Explosion.ShockProfilePlateauAndTapers`

- **Prepare:** Use the shock configuration from EX04 at age **0.2 s**. Place separate stationary bodies at the following +X distances: **0.5**, **1.0**, **1.5**, **2.0**, **2.5**, **3.0**, **3.5**, **4.0**, **4.5 m**.
- **Run:** Evaluate and integrate each body once.
- **Check:** The inferred outward X forces, in that same distance order, are **0**, **0**, **40**, **80**, **80**, **80**, **40**, **0**, **0 N**. All components other than X are **0 N**. Every comparison uses the common force tolerance.

## EX06 — `Explosion.ShockFadesAndStops`

- **Prepare:** Disable implosion and convection. Configure shock duration **1 s**, speed **0.1 m/s**, thickness **4 m**, and peak force **100 N**. Keep the reference taper setting. Place a stationary body at **(1, 0, 0)** in independent fixtures with ages **0 s**, **0.5 s**, and **1 s**.
- **Run:** Evaluate and integrate once in each fixture.
- **Check:** Inferred X forces are **+100 N**, **+50 N**, and **0 N**, respectively. Y and Z forces are **0 N**. No shock force remains at the phase-end boundary.

## EX07 — `Explosion.ShockRespondsToRadialVelocity`

- **Prepare:** Use the shock configuration from EX04 at age **0.2 s**. Place a body at **(2.5, 0, 0)**. Repeat in independent fixtures with initial X velocities **0**, **+5**, **-5**, **+10**, **+15**, and **-50 m/s**. Also test initial velocity **(0, +5, 0) m/s**.
- **Run:** Apply the force and integrate once in each fixture. Measure the change relative to each body's own initial velocity.
- **Check:** Inferred X forces for the six X-velocity cases are **+80**, **+40**, **+120**, **0**, **0**, and **+240 N**, respectively. The tangential-velocity case produces **+80 N** along X, **0 N** along Y and Z, and does not directly alter its Y velocity. No shock force points inward.

## EX08 — `Explosion.ConvectionActsOnlyInUpperCylinder`

- **Prepare:** Disable implosion and shock. Configure convection duration **2 s**, peak force **20 N**, chimney radius **2 m**, and chimney height **4 m**. At age **0 s**, test separate bodies at **(0, 1, 0)**, **(0, 0, 0)**, **(2, 1, 0)**, **(2.1, 1, 0)**, **(0, -0.1, 0)**, **(0, 4, 0)**, and **(0, 4.1, 0)**.
- **Run:** Evaluate and integrate each case once.
- **Check:** Both the first and second positions receive **(0, +20, 0) N**. All other positions receive **(0, 0, 0) N**. Horizontal force components are zero in every case. In particular, convection still acts at the detonation center even though radial forces there are zero.

## EX09 — `Explosion.ConvectionDecayAndPhaseOverlap`

- **Prepare:** First, use the convection configuration from EX08. Test fresh bodies at **(0, 1, 0)** and **(1, 1, 0)** at age **0 s**, and a body at **(0, 1, 0)** at ages **1 s** and **2 s**. Second, use a combined explosion with implosion duration **0.1 s**, implosion minimum radius **0.2 m**, implosion maximum radius **3 m**, implosion force **12 N**, shock duration **1 s**, shock speed **10 m/s**, shock thickness **1 m**, shock peak force **100 N**, convection duration **2 s**, convection peak force **20 N**, chimney radius **2 m**, and chimney height **4 m**; set its age to **0.3 s** and use a stationary body at **(0, 1.5, 0)**.
- **Run:** Evaluate and integrate each independent case once.
- **Check:** The four convection-only fixtures produce upward forces of **20 N**, **10 N**, **10 N**, and **0 N**. The combined fixture produces **(0, +58, 0) N**, showing that the shock and convection contributions are both applied. The X and Z forces are **0 N** in every fixture.

## EX10 — `Explosion.SameForceProducesInverseMassResponse`

- **Prepare:** Use the shock configuration from EX04 at age **0.2 s**. In independent fixtures place bodies at **(2.5, 0, 0)** with masses **1 kg**, **2 kg**, and **infinite mass** (zero inverse mass).
- **Run:** Evaluate the explosion and integrate once per fixture using the common step.
- **Check:** X velocity increases are **1.333333 m/s** for 1 kg and **0.666667 m/s** for 2 kg, within the common velocity tolerance. The infinite-mass body remains fixed; no result is non-finite.

## EX11 — `Explosion.ClockIsIndependentOfBodyCount`

- **Prepare:** Use the shock configuration from EX04 at age **0.2 s**. Register two 1 kg stationary bodies at **(2.5, 0, 0)** with one `Explosion` through a real `ForceRegistry`. Prepare a second independent fixture with the same two target bodies plus eight more registered bodies at **(100, 0, 0)**.
- **Run:** Call `ForceRegistry::updateForces()` once for **1/60 s**, integrate each dynamic body exactly once, read the explosion age, then advance that explosion's clock explicitly once by **1/60 s**. Repeat for both fixtures.
- **Check:** Each target body's X velocity increases by **1.333333 m/s** in both fixtures. The explosion age remains **0.2 s** until the explicit advance; afterward it is **0.216667 s** within the common age tolerance. Registering eight extra bodies changes neither the force nor the clock.

## EX12 — `ForceRegistry.RemoveOnlyMatchingPair`

- **Prepare:** Create two shock-only generators, **A** and **B**, each at age **0 s**, with shock duration **1 s**, speed **10 m/s**, thickness **1 m** and no other active phases. A has a peak force of **10 N**; B has **20 N**. Create two stationary 1 kg bodies, each at **(0.5, 0, 0)**. Add registrations **(body1, A)** twice, **(body1, B)** once, and **(body2, A)** once. Call `remove(body1, A)`.
- **Run:** Update the real registry for one fixed step and integrate each body once. Call `remove(body1, A)` again and repeat after restoring the bodies' initial positions and velocities and clearing their force accumulators.
- **Check:** On both updates, body1's inferred X force is **+20 N** and body2's inferred X force is **+10 N**. Both duplicate registrations of the removed pair are gone. Unrelated pairs remain registered, and the repeated removal is harmless.

## EX13 — `ForceRegistry.ClearForgetsButDoesNotDestroy`

- **Prepare:** Use the shock configuration from EX04 at age **0 s**. Register two stationary 1 kg bodies at **(0.5, 0, 0)** with a single live generator, then call `ForceRegistry::clear()`.
- **Run:** Update the cleared registry and integrate both bodies once. Subsequently apply the still-live generator directly to one body and integrate it once more.
- **Check:** Both bodies receive **0 N** from the cleared registry. The bodies and generator remain usable. Directly reusing the generator produces an X force of **+100 N** within tolerance.

## EX14 — `Lifetime.UnregisterBeforeMidBlastDeletion`

- **Prepare:** Create two initialized, heap-owned dynamic `RigidBody` instances, one active `Explosion`, and a real `ForceRegistry`; register each body with that explosion. Optionally register the bodies with `PhysicsWorld` using valid colliders.
- **Run:** Execute one force/integration tick. Remove all registrations involving the first body, unregister its physics registration if present, then destroy it. Continue for **at least 30 ticks** on the surviving body, advancing the explosion clock once per tick. Remove all generator registrations before destroying the explosion. Repeat the sequence with a full reset.
- **Check:** No deleted body or destroyed generator is referenced after removal. All surviving poses and velocities are finite; registered-force updates continue without failure. The registry contains no stale registrations after cleanup or reset. Run under ASan/UBSan when available; do not intentionally dereference a deleted object.

## EX15 — `SiegeLab.ExplosiveImpactCanCauseIndirectCageDamage`

- **Prepare:** Build a headless Siege Lab fixture with its existing launcher, fixed cage, and at least one movable rigid block placed so it can strike the cage. Create two variants that differ **only** by projectile type (standard or explosive). Choose and document fixed body masses, positions, launch settings, explosion parameters, and an upper step limit. Preserve the original cage damage threshold and `CollisionEnter` policy.
- **Run:** Simulate both variants using the genuine Cyclone detectors and solver at **60 fixed steps per second**. Record actual contact events, detonation count, cage state and deferred removals. Perform a complete reset and repeat the explosive variant with identical initial settings.
- **Check:** The standard shot creates **zero** explosions and leaves the cage intact. The explosive shot detonates **exactly once** following a significant projectile impact. The cage is destroyed only following a genuine **block–cage `CollisionEnter`** that satisfies its existing damage threshold, never by an explosion radius check. The result is reproduced after reset. No double integration, stale entity IDs, dangling force registrations or unsafe mid-solver deletions occur.
