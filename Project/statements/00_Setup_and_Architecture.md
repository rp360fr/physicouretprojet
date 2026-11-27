# Siege Lab — 00. Setup & Architecture

**Year 2 · Pairs · C++20 · Cyclone · raylib · Visual Studio · GoogleTest**

## The game

Aim a fixed launcher through an orbital camera and fire Cyclone-simulated projectiles. Preview the real path with an isolated ghost simulation. Topple a **3 m pillar supporting a cube**; destroy a **single solid cage block** either directly or with the falling cube. Win/lose states and a few sounds complete the game.

**Rules:** All motion/collisions use Cyclone; no animated fake collapses. One cage = one fixed `RigidBody` + one `CollisionBox`, no bird body, bars, joints, ropes, fragments or kinematic obstacle.

## Controls and reference scene

Use a fixed world convention throughout gameplay and tests. The camera never determines the launcher direction.

| Item | Reference |
|---|---|
| Axes / ground | +X toward tower; +Y up; +Z depth; ground `y=0` |
| Launcher | `(-8,1,0)` m; yaw/pitch in Euler angles |
| Camera | RMB drag orbit; mouse wheel zoom; independent from launcher |
| Launcher keys | `A/D` yaw; `W/S` pitch; `Q/E` speed; `1/2` projectile; Space fire |
| Game keys | Enter start; R reset/menu |
| Physics | Fixed `dt=1/60 s`, gravity `(0,-9.81,0) m/s²` |
| Collision events | `Enter` and `Exit` only; quiet resting contacts remain silent |

## Solution setup (30 min)

Use [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart) and its Visual Studio generation script. Pin the supplied Cyclone archive and one raylib revision so everyone builds the same code.

1. Run `build-VisualStudio2022.bat` or `build-VisualStudio2026.bat` as appropriate. Update `build/premake5.lua`; select **x64**, Debug/Release and **C++20**.
2. Configure `Cyclone` (static library), `SiegeLabCore` (static library), `SiegeLabGame` (executable), `SiegeLabTests` (GoogleTest executable). The quickstart raylib project may remain alongside them.
3. Convert the template entry point to `.cpp`; link raylib with the game/core as required. Install GoogleTest reproducibly; the quickstart does not supply it.
4. Build all targets, open one raylib window, then run a headless GoogleTest assertion: `2+3==5`.

**Files:** `build/premake5.lua`, `src/main.cpp`, supplied Cyclone `include/` and `src/`. **Cyclone edits:** none.

```mermaid
flowchart LR
    C[Cyclone static library] --> S[SiegeLabCore static library]
    R[raylib library] --> S
    S --> G[SiegeLabGame executable]
    S --> T[SiegeLabTests executable]
    R --> G
    R --> T
    classDef lib fill:#DCFCE7,stroke:#16A34A,color:#14532D
    classDef core fill:#DBEAFE,stroke:#2563EB,color:#1E3A8A
    classDef exe fill:#E9D5FF,stroke:#9333EA,color:#581C87
    class C,R lib
    class S core
    class G,T exe
```

## Cyclone source map

Use the **teacher ZIP**. It contains flat `include/*.h` paths: stage them under `cyclone/` for source directives such as `<cyclone/body.h>`.

| Library | Use | Edit? |
|---|---|---|
| `include/body.h`, `src/body.cpp` | `RigidBody`, transform, integration | No |
| `include/core.h`, `src/core.cpp` | `Vector3`, quaternion, inertia | No |
| `include/collide_fine.h`, `src/collide_fine.cpp` | Sphere, box, plane, detectors | `src/collide_fine.cpp` in M7 only |
| `include/contacts.h`, `src/contacts.cpp` | `Contact`, `ContactResolver` | M7 observer/getter only |
| `include/world.h`, `src/world.cpp` | Existing Cyclone `World` example | Read only; not our `PhysicsWorld` |
| `include/fgen.h`, `src/fgen.cpp` | Optional force generators | Read only |

**Safety:** `CollisionPrimitive` has no virtual destructor; `CollisionPlane` is not derived from it. Never delete colliders via `CollisionPrimitive*`. Do not mistake `RigidBody::integrate()` for contact solving.

## Gameplay orchestration

Game flow and camera selection are independent of physics internals. Only `OrbitCamera` is required now; `FreeCamera` is a future extension, not a student deliverable.

```mermaid
classDiagram
    direction LR
    class GameController {
        +processInput()
        +fixedTick(dt)
        +restart()
    }
    class InputController {
        +capture() InputSnapshot
    }
    class GameStateMachine {
        +transition(next)
        +update(dt)
    }
    class IGameState {
        <<interface>>
        +onEnter(context)*
        +onExit(context)*
        +update(context, dt)*
    }
    class StartState
    class PlayingState
    class GameOverState
    class CameraManager {
        +setActive(controller)
        +update(input, dt)
        +getCamera() Camera3D
    }
    class ICameraController {
        <<interface>>
        +update(input, dt)*
        +getCamera() Camera3D*
        +reset()*
    }
    class OrbitCamera {
        -yaw
        -pitch
        -rotationSpeed
        -distance
    }
    class FreeCamera {
        <<future>>
    }
    class Launcher {
        -yaw
        -pitch
        -speed
        +shotSettings() ShotSettings
    }
    class ShotSettings {
        +origin
        +direction
        +speed
        +projectileKind
    }
    class TrajectoryPredictor {
        +predictUntilContact(settings)
    }
    GameController *-- GameStateMachine
    GameController --> InputController
    GameController --> CameraManager
    GameController --> Launcher
    GameController --> TrajectoryPredictor
    GameStateMachine o-- IGameState
    IGameState <|.. StartState
    IGameState <|.. PlayingState
    IGameState <|.. GameOverState
    CameraManager o-- ICameraController
    ICameraController <|.. OrbitCamera
    ICameraController <|.. FreeCamera
    Launcher --> ShotSettings
    TrajectoryPredictor ..> ShotSettings
    style GameController fill:#DBEAFE,stroke:#2563EB,color:#1E3A8A,stroke-width:2px
    style GameStateMachine fill:#BFDBFE,stroke:#2563EB,color:#1E3A8A
    style InputController fill:#DDD6FE,stroke:#7C3AED,color:#4C1D95
    style IGameState fill:#E9D5FF,stroke:#9333EA,color:#581C87
    style StartState fill:#BBF7D0,stroke:#16A34A,color:#14532D
    style PlayingState fill:#FED7AA,stroke:#EA580C,color:#7C2D12
    style GameOverState fill:#FBCFE8,stroke:#DB2777,color:#831843
    style CameraManager fill:#67E8F9,stroke:#0891B2,color:#164E63
    style ICameraController fill:#CFFAFE,stroke:#0891B2,color:#164E63
    style OrbitCamera fill:#A5F3FC,stroke:#0891B2,color:#164E63
    style FreeCamera fill:#E2E8F0,stroke:#64748B,color:#334155
    style Launcher fill:#FDBA74,stroke:#EA580C,color:#7C2D12
    style ShotSettings fill:#FEF3C7,stroke:#D97706,color:#713F12
    style TrajectoryPredictor fill:#FDE68A,stroke:#CA8A04,color:#713F12
```

## Entity inheritance and rendering

`GameEntity` owns a body, `Projectile` composes a `CollisionSphere`, and `Block` composes a `CollisionBox`. Concrete types set their own mass, size and appearance. Render through `GameEntity::draw()` with raylib primitives and `rlgl` rotation.

```mermaid
classDiagram
    direction LR
    class GameEntity {
        <<abstract>>
        -id : EntityId
        -body : RigidBody
        +configurePhysics()*
        +registerPhysics(world)*
        +draw()*
        +virtualDestructor()
    }
    class Projectile {
        <<abstract>>
        -sphere : CollisionSphere
        +fire(settings)
        +registerPhysics(world)
        +draw()
    }
    class StandardProjectile {
        +configurePhysics()
    }
    class HeavyProjectile {
        +configurePhysics()
    }
    class Block {
        <<abstract>>
        -box : CollisionBox
        +registerPhysics(world)
        +draw()
    }
    class PillarBlock {
        +configurePhysics()
    }
    class CubeBlock {
        +configurePhysics()
    }
    class CageBlock {
        +configurePhysics()
        +draw()
        +breakThreshold : real
    }
    class SceneObjects {
        +createEntity(args) EntityId
        +get(id) GameEntityPtr
        +requestRemove(id)
        +flushRemovals()
    }
    class ObjectPool {
        <<template>>
        +create(args)
        +get(id)
        +destroy(id)
    }
    class SceneRenderer {
        +drawEntities()
        +drawHUD()
        +drawTrajectory()
    }
    class DamageSystem {
        +onCollision(event)
    }
    GameEntity <|-- Projectile
    GameEntity <|-- Block
    Projectile <|-- StandardProjectile
    Projectile <|-- HeavyProjectile
    Block <|-- PillarBlock
    Block <|-- CubeBlock
    Block <|-- CageBlock
    SceneObjects *-- ObjectPool
    ObjectPool o-- GameEntity : owns polymorphic entities
    SceneRenderer --> SceneObjects : virtual drawing
    DamageSystem --> CageBlock : requests destruction
    style GameEntity fill:#C7D2FE,stroke:#4338CA,color:#312E81,stroke-width:2px
    style Projectile fill:#FED7AA,stroke:#EA580C,color:#7C2D12,stroke-width:2px
    style StandardProjectile fill:#FDBA74,stroke:#F97316,color:#7C2D12
    style HeavyProjectile fill:#FB923C,stroke:#C2410C,color:#7C2D12
    style Block fill:#DBEAFE,stroke:#2563EB,color:#1E3A8A,stroke-width:2px
    style PillarBlock fill:#93C5FD,stroke:#1D4ED8,color:#1E3A8A
    style CubeBlock fill:#A5B4FC,stroke:#4F46E5,color:#312E81
    style CageBlock fill:#FECACA,stroke:#DC2626,color:#7F1D1D,stroke-width:2px
    style SceneObjects fill:#CBD5E1,stroke:#475569,color:#1E293B
    style ObjectPool fill:#E2E8F0,stroke:#64748B,color:#334155
    style SceneRenderer fill:#F9A8D4,stroke:#BE185D,color:#831843
    style DamageSystem fill:#FDE68A,stroke:#CA8A04,color:#713F12
```

**Ownership:** `SceneObjects` owns `ObjectPool<std::unique_ptr<GameEntity>, N>` and a C++20-constrained `createEntity<T>()`. The objects must keep stable addresses while registered. No `variant`, `dynamic_cast`, geometry-specific renderer switches or mandatory meshes.

## PhysicsWorld and event boundaries

`PhysicsWorld` borrows existing bodies and typed colliders. It detects/resolves contacts, copies prepared speeds and emits **impact episodes**; gameplay may only request deletion after the step.

```mermaid
classDiagram
    direction LR
    class PhysicsWorld {
        +registerSphere(id, body, sphere)
        +registerBox(id, body, box)
        +registerPlane(plane)
        +unregisterObject(id)
        +step(dt)
        +reset()
    }
    class PhysicsRegistration {
        +entityId
        +body : RigidBodyPtr
        +collider : CollisionPrimitivePtr
        +kind : SphereOrBox
    }
    class ContactSnapshot {
        +entityA
        +entityB
        +point
        +normal
        +closingSpeed
    }
    class CollisionEventGenerator {
        +generate(snapshots)
        +reset()
    }
    class CollisionEventDispatcher {
        +subscribe(listener)
        +unsubscribe(token)
        +publish(events)
    }
    class ICollisionListener {
        <<interface>>
        +onCollision(event)*
    }
    class CollisionEvent {
        +phase : EnterOrExit
        +entityA
        +entityB
        +impactSpeed
    }
    class CycloneContactResolver {
        <<external>>
        +observePreparedContacts()
    }
    PhysicsWorld o-- PhysicsRegistration : borrows
    PhysicsWorld --> CycloneContactResolver
    PhysicsWorld --> ContactSnapshot : copies
    PhysicsWorld *-- CollisionEventGenerator
    PhysicsWorld *-- CollisionEventDispatcher
    CollisionEventGenerator --> CollisionEvent : emits
    CollisionEventDispatcher --> ICollisionListener : notifies
    style PhysicsWorld fill:#86EFAC,stroke:#15803D,color:#14532D,stroke-width:2px
    style PhysicsRegistration fill:#BBF7D0,stroke:#16A34A,color:#14532D
    style ContactSnapshot fill:#A5F3FC,stroke:#0891B2,color:#164E63
    style CollisionEventGenerator fill:#D8B4FE,stroke:#7E22CE,color:#581C87
    style CollisionEventDispatcher fill:#FBCFE8,stroke:#DB2777,color:#831843
    style CollisionEvent fill:#E9D5FF,stroke:#9333EA,color:#581C87
    style ICollisionListener fill:#FED7AA,stroke:#EA580C,color:#7C2D12
    style CycloneContactResolver fill:#1E3A8A,stroke:#0F172A,color:#FFFFFF
```

**Patch scope:** only `include/contacts.h`, `src/contacts.cpp`, `src/collide_fine.cpp` in M7. See [PhysicsWorld milestones](./02_PhysicsWorld_Milestones.md).

## Two-student integration

Build gameplay and the world in parallel. Gameplay may initially call Cyclone directly; at merge, replace the temporary loop with **one** `PhysicsWorld::step(dt)` and register the unchanged, already-owned objects.

```mermaid
flowchart LR
    S[SceneObjects owns entities] -->|borrows IDs and colliders| P[PhysicsWorld]
    P -->|Enter or Exit| D[CollisionEventDispatcher]
    D -->|notifies| H[DamageSystem and AudioFeedback]
    H -->|deferred removal| S
    S -->|unregister first| P
    classDef owner fill:#DBEAFE,stroke:#2563EB,color:#1E3A8A
    classDef phys fill:#DCFCE7,stroke:#16A34A,color:#14532D
    classDef events fill:#E9D5FF,stroke:#9333EA,color:#581C87
    class S owner
    class P phys
    class D events
    class H owner
```

## Fixed-step contract

There is one authoritative update sequence. The Cyclone observer sees prepared contacts **before solving**, but listeners are notified **after** solving; deletion always follows unregistration.

```mermaid
flowchart LR
    subgraph A["1 · Motion"]
        direction TB
        A1["Read commands"] --> A2["Create or wake bodies"]
        A2 --> A3["Integrate via Cyclone"]
    end
    subgraph B["2 · Detection"]
        direction TB
        B1["Refresh colliders"] --> B2["Check contact capacity"]
        B2 --> B3["Generate contacts"]
    end
    subgraph C["3 · Cyclone resolver"]
        direction TB
        C1["Prepare contacts internally"] --> C2["Observer copies snapshots"]
        C2 --> C3["Correct positions and velocities"]
        C3 --> C4["Refresh corrected body + collider transforms"]
    end
    subgraph D["4 · Gameplay"]
        direction TB
        D1["Build Enter or Exit"] --> D2["Notify listeners"]
        D2 --> D3["Unregister and destroy"]
        D3 --> D4["Update game state"]
    end
    A3 --> B1
    B3 --> C1
    C4 --> D1

    classDef motion fill:#DBEAFE,stroke:#2563EB,color:#1E3A8A,stroke-width:2px
    classDef detection fill:#BBF7D0,stroke:#16A34A,color:#14532D,stroke-width:2px
    classDef resolution fill:#D1FAE5,stroke:#047857,color:#064E3B,stroke-width:2px
    classDef events fill:#E9D5FF,stroke:#9333EA,color:#581C87,stroke-width:2px
    classDef gameplay fill:#FED7AA,stroke:#EA580C,color:#7C2D12,stroke-width:2px
    class A1,A2,A3 motion
    class B1,B2,B3 detection
    class C1,C3,C4 resolution
    class C2,D1 events
    class D2,D3,D4 gameplay
```

## Deliverables & acceptance

The application must demonstrate both direct and indirect cage destruction with genuine Cyclone collisions. Submit compilable code, tests and concise build notes; do not submit only screenshots.

1. Reproducible Visual Studio solution with pinned dependencies and `SiegeLabGame`/`SiegeLabTests`.
2. All 12 numbered milestones, the generic pool and 80 implemented GoogleTest cases (**55 Gameplay / 25 PhysicsWorld**).
3. `PillarBlock` + `CubeBlock` remain at rest; projectile dislodges them; `CageBlock` disappears when hit hard enough.
4. No double stepping, unsafe collider deletion, fake physics, event spam or stale listener subscriptions.

**Verification status:** Earlier teacher C++20 probes compiled Cyclone under GCC/Clang and exercised physical scenarios. **The finished student game, Windows build, raylib window/audio and all 80 student GoogleTests have not been executed.**