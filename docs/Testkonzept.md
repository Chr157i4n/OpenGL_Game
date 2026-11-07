# Testkonzept für OpenGL_Game

Ziel: ein nachhaltiges, CMake-integriertes Unit-Test-Setup, das die **testbare reine Logik** absichert, ohne einen OpenGL-/Fenster-/GPU-/Audio-Kontext zu benötigen.

---

## 1. Grundproblem und Strategie

Das Projekt trennt sich in zwei Teile mit sehr unterschiedlicher Testbarkeit:

| Bereich | Testbarkeit |
| --- | --- |
| `GameServer/` | **Vollständig headless** – keine OpenGL-, SDL-, Audio- oder GPU-Abhängigkeit. Ideal für echte Unit-Tests. |
| `Game/` | Vermischt reine Logik (Physik, Kamera, Parsing, UI-State) mit GL-Klassen (`Shader`, `Renderer`, `VertexBuffer`, `FrameBuffer`). Direkt nicht testbar. |

**Kernstrategie:** Wir testen die Logik, die ohne GL auskommt. Für den Client wird die reine Logik aus den GL-Klassen **herausgelöst** (Testbarkeit durch Entkopplung), statt zu versuchen, einen ganzen GL-Kontext nachzubauen.

### Test-Pyramide

```
        End-to-End / Smoke (manuell, server-only Build)
       Integration (Server<->Client-Protokoll, Asset-Roundtrips)
      Unit-Tests (Physik, Mathematik, Parser, State-Machines)   <- Hauptschwerpunkt
```

---

## 2. Framework-Wahl: **Catch2**

Empfehlung: [Catch2](https://github.com/catchorg/Catch2) (v3.x).

**Warum Catch2?**
- Header-/library-basiert, passt perfekt zum bestehenden `FetchContent`-Muster.
- Natürliche, lesbare Assertions (`REQUIRE(x == Approx(1.0))`) – gut für GLM-Vektormathematik.
- Eingebauter Support für Approximate-Comparisons (wichtig für floats/Vektoren).
- Sektionierung von Tests, Benchmarks, tags.

Alternative: **doctest** (noch leichter, einziger Header). Für dieses Projekt ist Catch2 die bessere Wahl wegen `Approx` und etablierter CMake-Unterstützung.

Integration in `cmake/Dependencies.cmake` (analog zu bestehenden Abhängigkeiten):

```cmake
# ---------------------------------------------------------------------------
# Catch2 (nur für Tests)
# ---------------------------------------------------------------------------
option(OG_BUILD_TESTS "Build unit tests" ON)

if(OG_BUILD_TESTS)
    if(OG_USE_SYSTEM_DEPS)
        find_package(Catch2 3 CONFIG QUIET)
    endif()

    if(NOT TARGET Catch2::Catch2WithMain)
        FetchContent_Declare(catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.7.1
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(catch2)
    endif()
endif()
```

---

## 3. Testbare Module (priorisiert)

### Ebene A – sofort testbar, keine Entkopplung nötig

Diese Klassen haben **keine GL-Abhängigkeit** und können direkt getestet werden:

| Modul | Testbare Funktionen | Typ |
| --- | --- | --- |
| `GameServer/Helper.h/.cpp` | `split`, `eraseSubStr`, `to_string_with_precision`, `glmVec3_to_string` / `string_to_glmVec3` (Round-Trip) | String-/Vektor-Mathematik |
| `Game/Helper.h/.cpp` | `split`, `trim`, `eraseSubStr`, `glmVec{2,3,4}_to_string` / `string_to_glmVec{2,3,4}` | String-/Vektor-Mathematik |
| `GameServer/Object.h/.cpp` | `checkCollision_AABB`, `checkCollision_SAT` (MTV), `intersectWithRay`, `calculateDimensions`, `calculateRectPoints`, `calculateCollisionPoints` (Würfel/Prisma), `projectedOverlap`, `convertStringToType` | Physik/Kollision (SAT) |
| `Game/Raypicker.h/.cpp` | `doRayPicking` (Slab-Intersektion), `getNearestObject` | Ray/AABB-Mathematik |
| `Game/Camera.h/.cpp`, `Camera_FPS`, `Camera_Floating` | `getProj`, `getView`, `getViewProj`, `translate`, `onMouseMoved` (Pitch-Clamp ±60°), `moveSideways` (Cross-Produkt) | Matrix-/Trig-Mathematik |
| `GameServer/Model.h/.cpp` | `init(filename)` BMF-Parser, `getDimension`, `getBoundingBoxDimension`, `getHasTransparentTexture` | Binary-Parser (Golden-File) |
| `GameServer/ConfigManager` | `readConfig`, `writeConfig`, Kommentar-/Section-Skipping, Missing-Key-Append | INI-Parser |
| `GameServer/StopWatch`, `Game/StopWatch` | `start`, `stop` | Zeitmessung |
| `GameServer/Logger` | Log-Formatierung `[hh:mm:ss] message` | String-Format |
| `GameServer/Map`, `Game/Map` | `getGravity`, `getMapSize`, `getSkyboxFaces`, Filename-Normalisierung in `load()` | reine Getter + Parsing |
| `GameServer/Character`, `Bullet`, `NPC`, `Client` | `run`, `crouch`, `jump`, `resetVerticalMovement`, `getLookOrigin`, `forwardSpeed`-Verhalten, `Bullet::fall`-Rotationsformel, `NPC`-Nav-Point-Zyklus | State-Machine / Physik |
| `GameServer/ResourceManager` | `loadAllModels` (Dedup), `readAllMaps`, XML-Map-Parsing | tinyxml2-Parser |

### Ebene B – Logik extrahierbar, aber GL-Kopplung entfernen

Hier muss die reine Logik von GL getrennt werden, damit sie testbar wird:

| Modul | Problem | Lösung |
| --- | --- | --- |
| `UI_Element` + `UI_Element_*` | Header inkludiert `<SDL.h>`, `isMouseOver` braucht `Game::getWindowHeight()` | Getter/Setter + `isStillAlive()` + Slider-Clamping + Button-`nextState` testbar, wenn GL-Fenster nicht benötigt wird. `isMouseOver` nur mit injizierter Fenstergröße. |
| `Game/ConfigManager` | `#include "Game.h"` zieht GLEW+SDL nach | Server-Kopie verwenden **oder** `Game.h`-Include entfernen. |
| `Game/Model` | `init()` ruft `glGenTextures`, `stbi_load` | Nur `Mesh::calculateDimension` / `getDimension` extrahieren (GL-freier Teil). |
| `Game/Character`, `Bullet`, `Player` | Konstruktoren benötigen `Shader*` | Physik-/Bewegungslogik in GL-freie Basisklasse oder freie Funktionen auslagern. |
| `Game/NetworkManager`, `GameServer/NetworkManager` | `parseData` braucht `<enet/enet.h>` | Framing-Logik (`"NN|id|payload"`, `networkIDtoString`, `getClientID`) als GL-/enet-freie freie Funktionen isolieren. |

### Ebene C – NICHT unit-testbar (nur Integration/Smoke)

`Shader`, `Renderer`, `VertexBuffer`, `IndexBuffer`, `FrameBuffer`, `Font`, `Font3D`, `AudioManager`, `Game`/`main`, `GameMode_*`, `Menu_*`, `UI` (Zeichenteil). Diese erfordern einen echten GL-Kontext und werden allenfalls als Headless-Smoke-Test (server-only Build) geprüft.

---

## 4. Architektur der Teststruktur

```
tests/
├── CMakeLists.txt
├── helpers/
│   └── test_helpers.h          # glm-Vektor-Approx, Fixture-Helfer
├── core/
│   ├── test_helper.cpp
│   ├── test_camera.cpp
│   ├── test_config_manager.cpp
│   └── test_stopwatch.cpp
├── physics/
│   ├── test_object_collision.cpp
│   ├── test_object_bounds.cpp
│   └── test_raypicker.cpp
├── assets/
│   ├── test_model_bmf.cpp
│   └── test_resource_manager.cpp
├── protocol/
│   └── test_network_framing.cpp
└── data/
    ├── levels/level_test.xml   # Kopie oder Referenz
    └── models/testball01.bmf   # Golden-Files
```

**Prinzipien:**
1. **Test gegen `GameServer`-Code zuerst** – dort ist alles headless.
2. Für Client-Logik: **reine Funktionen frei testbar machen**, statt GL-Klassen instanziieren.
3. **Golden-Files** für Parser (`Model::init`, `ResourceManager::loadMap`) gegen die vorhandenen Assets.
4. **Keine echten Netzwerkverbindungen** – nur Framing/Parsing isolieren.

---

## 5. CMake-Integration der Tests

In `CMakeLists.txt` (am Ende, nach den Executable-Blöcken):

```cmake
if(OG_BUILD_TESTS)
    add_executable(og_tests ${TEST_SOURCES})
    target_include_directories(og_tests PRIVATE
        "${CMAKE_SOURCE_DIR}/Game"
        "${CMAKE_SOURCE_DIR}/GameServer"
        "${CMAKE_SOURCE_DIR}/common")

    # Nur die testbaren Quelldateien einbinden, nicht die GL-/SDL-Kompiliereinheiten
    target_sources(og_tests PRIVATE
        "${CMAKE_SOURCE_DIR}/GameServer/Object.cpp"
        "${CMAKE_SOURCE_DIR}/GameServer/Helper.cpp"
        "${CMAKE_SOURCE_DIR}/GameServer/ConfigManager.cpp"
        "${CMAKE_SOURCE_DIR}/GameServer/Model.cpp"
        "${CMAKE_SOURCE_DIR}/GameServer/StopWatch.cpp"
        "${CMAKE_SOURCE_DIR}/GameServer/Logger.cpp")

    target_link_libraries(og_tests PRIVATE
        Catch2::Catch2WithMain
        glm::glm
        tinyxml2::tinyxml2)

    find_package(Catch2 3 REQUIRED)
    include(Catch)
    catch_discover_tests(og_tests)
endif()
```

**Wichtiger Hinweis:** Der Server-Code hat **keine** OpenGL-/SDL-Abhängigkeit. Deshalb kann die Test-Binary nur `glm`, `tinyxml2`, `enet` (falls Protokoll getestet wird) und Catch2 verlinken – **kein GLEW, kein SDL, kein SoLoud**. Das ist der entscheidende Vorteil: Tests laufen auf jedem CI-Runner ohne GPU.

---

## 6. Beispieldaten für Test fixtures

Die vorhandenen Assets sind perfekt als Fixtures:

- `GameServer/models/testball01.bmf` … `testball10.bmf` → BMF-Parser-Golden-Files
- `GameServer/levels/level_test.xml` → Map-/ResourceManager-Parsing
- `GameServer/config.ini` → ConfigManager
- `GameServer/models/character.bmf` → Character-Physik-Dimensionen

---

## 7. Konkrete Testfälle (Auswahl)

### Helper (String-/Vektor-Mathematik)
```cpp
TEST_CASE("split zerlegt String") {
    std::vector<std::string> out;
    size_t n = Helper::split("a;b;c", out, ';');
    REQUIRE(n == 3);
    REQUIRE(out[0] == "a");
    REQUIRE(out[2] == "c");
}

TEST_CASE("glmVec3 Round-Trip") {
    glm::vec3 v{1.2f, -3.4f, 5.6f};
    std::string s = Helper::glmVec3_to_string(v);
    glm::vec3 back = Helper::string_to_glmVec3(s);
    REQUIRE(back.x == Approx(v.x).epsilon(1e-3f));
}
```

### Object – Collision (SAT)
```cpp
TEST_CASE("AABB-Broadphase erkennt Überlappung") {
    Object a("cube.bmf"), b("cube.bmf");
    a.setPosition({0,0,0}); b.setPosition({1,0,0});
    REQUIRE(a.checkCollision_AABB(std::make_shared<Object>(b)) == false); // noch getrennt
    b.setPosition({0.5f,0,0});
    REQUIRE(a.checkCollision_AABB(std::make_shared<Object>(b)) == true);  // überlappen
}

TEST_CASE("projectedOverlap") {
    REQUIRE(projectedOverlap(0, 10, 5, 15) == 5);
    REQUIRE(projectedOverlap(0, 1, 2, 3) == 0);
}
```

### Camera
```cpp
TEST_CASE("Perspective-Projection") {
    Camera cam(60.0f, 800, 600);
    glm::mat4 proj = cam.getProj();
    // glm::perspective Vergleich
    REQUIRE(proj[3][3] == Approx(1.0f));
}

TEST_CASE("Pitch-Clamp bei FPSCamera") {
    // onMouseMoved mit extremem yRel -> Pitch in [-60, 60]
}
```

### Model (BMF-Parser)
```cpp
TEST_CASE("BMF Round-Trip Dimensionen") {
    Model m;
    REQUIRE(m.init("models/testball01.bmf"));
    glm::vec3 d = m.getDimension();
    REQUIRE(d.x > 0.0f && d.y > 0.0f && d.z > 0.0f);
    REQUIRE(m.getHasTransparentTexture() == false); // .jpg
}
```

### ConfigManager
```cpp
TEST_CASE("Kommentarzeilen werden ignoriert") {
    ConfigManager::init("fixtures/config.ini");
    // "# comment" und "; comment" duerfen nicht als Key gelesen werden
}
```

---

## 8. CI-Integration

- **Build-Matrix:** Linux + Windows (MSVC + MinGW).
- Tests laufen im **server-only Modus** (`-DOG_BUILD_GAME=OFF -DOG_BUILD_TESTS=ON`), damit keine GPU/X11 nötig ist.
- CI-Schritt:
  ```bash
  cmake -S . -B build -DOG_BUILD_GAME=OFF -DOG_BUILD_TESTS=ON
  cmake --build build
  ctest --test-dir build
  ```
- Optional: **Sanitizer**-Variante (die `build-linux-asan`-Konfiguration zeigt bereits ASan-Interesse) für Object/Collision/Model-Speichertest.

---

## 9. Umsetzung in Phasen (empfohlene Reihenfolge)

1. **Phase 1 – Setup:** Catch2 via FetchContent, `tests/`-Verzeichnis, `test_helper.h` mit glm-Approx-Helfern.
2. **Phase 2 – Low-Hanging-Fruit:** `Helper`, `StopWatch`, `Logger`, `ConfigManager` (Server-Kopie) – keine Entkopplung nötig.
3. **Phase 3 – Physik:** `Object`-Collision (AABB/SAT), `projectedOverlap`, `intersectWithRay`, `Raypicker`.
4. **Phase 4 – Parser/Golden-Files:** `Model::init`, `ResourceManager::loadMap` gegen vorhandene Assets.
5. **Phase 5 – Kamera & State-Machines:** `Camera`/`FPSCamera`-Mathematik, `Character`-Speed/Crouch, `Bullet::fall`, `NPC`-Navigation, `UI_Element`-State.
6. **Phase 6 – Protokoll-Framing:** `NetworkManager`-Framing/Parsing als isolierte reine Funktionen.
7. **Phase 7 – Entkopplung:** Client-Klassen (ConfigManager, UI, Character) GL-frei refactorieren für breitere Abdeckung.

---

## 10. Zusammenfassung der Empfehlungen

- **Framework:** Catch2 (FetchContent), `catch_discover_tests`.
- **Schwerpunkt:** `GameServer`-Logik (headless) zuerst – höchster Wert, geringste Reibung.
- **Client:** reine Logik (Helper, Camera, Object-Physik, Parser) aus GL-Klassen herauslösen, statt GL-Kontext zu simulieren.
- **Fixtures:** vorhandene BMF-/XML-/INI-Assets als Golden-Files.
- **CI:** server-only Build ohne GPU; `ctest` in Build-Matrix Linux/Windows.
- **Keine echten Netzwerk-/GL-/Audio-Tests** im Unit-Scope; diese als Integration/Smoke separat.
