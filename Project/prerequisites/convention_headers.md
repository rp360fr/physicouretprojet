# Fichiers d'en-tête (Header Files)

En règle générale, chaque fichier d'implémentation (`.cc` ou `.cpp`) possède un
fichier d'en-tête (`.h`) associé. Les petits programmes contenant uniquement
`main()` et les fichiers de tests peuvent faire exception.

## 1. En-têtes autonomes (Self-contained Headers)

- Tout fichier d'en-tête porte l'extension `.h`.
- Un fichier `.h` doit pouvoir être inclus sans préparation particulière.
- Il doit posséder une protection contre les inclusions multiples.
- Il doit inclure directement tous les en-têtes dont il dépend.
- Les définitions des fonctions templates doivent être accessibles depuis
  l'en-tête où elles sont déclarées, sauf si leurs instanciations sont limitées
  à un fichier d'implémentation.
- Ne pas répartir les définitions dans des fichiers `-inl.h` inclus séparément.
- Réserver l'extension `.inc` aux rares fichiers destinés à être insérés dans
  un autre fichier et qui ne constituent pas des en-têtes autonomes.

**Exemple :**

```cpp
// modele/eleve.h
#ifndef COURS_MODELE_ELEVE_H_
#define COURS_MODELE_ELEVE_H_

#include <string>

struct Eleve {
  std::string nom;
};

#endif  // COURS_MODELE_ELEVE_H_
```

## 2. Protection par `#define` (The #define Guard)

- Tout fichier `.h` doit être protégé par `#ifndef`, `#define` et `#endif`.
- Le nom de la macro de protection est construit à partir du nom du projet,
  du chemin et du nom du fichier : `PROJET_CHEMIN_FICHIER_H_`.
- Cette macro doit être unique au sein du projet.

**Exemple :** pour `cours/modele/eleve.h` :

```cpp
#ifndef COURS_MODELE_ELEVE_H_
#define COURS_MODELE_ELEVE_H_

// Déclarations du fichier.

#endif  // COURS_MODELE_ELEVE_H_
```

## 3. Inclure ce que l'on utilise (Include What You Use)

- Inclure directement le fichier qui déclare chaque type, fonction ou symbole
  utilisé dans le fichier courant.
- Ne pas compter sur les inclusions indirectes d'un autre fichier.
- Ne pas ajouter d'en-tête qui n'est pas nécessaire.
- Cette règle s'applique aux fichiers `.h` comme aux fichiers `.cc` et `.cpp`.

**Correct :**

```cpp
// bulletin.h
#ifndef COURS_BULLETIN_H_
#define COURS_BULLETIN_H_

#include <string>
#include <vector>

struct Bulletin {
  std::string nom;
  std::vector<int> notes;
};

#endif  // COURS_BULLETIN_H_
```

**Incorrect :**

```cpp
#include <vector>  // Ne garantit pas l'accès à std::string.

struct Bulletin {
  std::string nom;  // Il manque #include <string>.
  std::vector<int> notes;
};
```

## 4. Déclarations anticipées (Forward Declarations)

- Préférer l'inclusion de l'en-tête qui déclare un type à une déclaration
  anticipée.
- Ne pas faire de déclaration anticipée pour les éléments d'une bibliothèque
  externe ou de la bibliothèque standard (`std`).
- Pour un type appartenant au projet, une déclaration anticipée peut être
  utilisée de manière exceptionnelle, si elle présente un intérêt clair.
- Une déclaration anticipée ne remplace pas la définition complète lorsque
  celle-ci est nécessaire (création d'objet, accès aux membres, héritage, etc.).

**À privilégier :**

```cpp
#include "modele/eleve.h"

void afficher(const Eleve& eleve);
```

**À éviter sans raison particulière :**

```cpp
class Eleve;

void afficher(const Eleve& eleve);
```

**Interdit :**

```cpp
namespace std {
class string;  // Ne jamais déclarer soi-même un type de std.
}
```

## 5. Définir des fonctions dans les en-têtes

*(Defining Functions in Header Files)*

- Ne définir une fonction directement dans sa déclaration publique que si
  elle est courte (environ 10 lignes maximum).
- Placer les fonctions plus longues dans un fichier `.cc` ou `.cpp`, sauf
  contrainte technique (notamment les templates).
- Si une définition doit se trouver dans un `.h`, mais qu'elle est longue,
  la placer dans une zone distincte des déclarations publiques.
- Une fonction ordinaire définie dans un en-tête doit être `inline` lorsque
  cela est nécessaire pour respecter la règle de définition unique (ODR).
- Les fonctions définies dans le corps d'une classe sont implicitement `inline`.
  Les fonctions templates suivent des règles spécifiques de définition multiple.
- Dans tous les cas, une fonction ne dépasse pas **25 lignes** et une ligne
  de code ne dépasse pas **80 caractères**.

**Correct : fonction très courte définie dans la classe.**

```cpp
class Compteur {
 public:
  int valeur() const { return valeur_; }
  void incrementer();

 private:
  int valeur_ = 0;
};
```

**Correct : fonction ordinaire définie dans le `.cpp`.**

```cpp
#include "compteur.h"

void Compteur::incrementer() {
  ++valeur_;
}
```

**Correct : fonction libre définie dans un en-tête.**

```cpp
inline int carre(int nombre) {
  return nombre * nombre;
}
```

**Correct : template défini dans le `.h`.**

```cpp
template <typename T>
T maximum(T a, T b) {
  return a < b ? b : a;
}
```

## 6. Noms et ordre des inclusions (Names and Order of Includes)

Dans un fichier d'implémentation, ranger les `#include` dans cet ordre :

1. En-tête associé au fichier (`.h` correspondant au `.cc` ou `.cpp`).
2. En-têtes système C (`<stddef.h>`, `<unistd.h>`, etc.).
3. En-têtes de la bibliothèque standard C++ (`<string>`, `<vector>`, etc.).
4. En-têtes des bibliothèques externes.
5. En-têtes du projet.

- Séparer chaque groupe non vide par une ligne blanche.
- Classer les inclusions par ordre alphabétique à l'intérieur de chaque groupe.
- Écrire les chemins relatifs à la racine des sources du projet, sans `./`
  ni `../`.
- Employer les chevrons `<...>` pour les en-têtes standard et système,
  ainsi que pour les bibliothèques externes qui l'exigent.
- Employer les guillemets `"..."` pour les en-têtes du projet.
- Garder les inclusions conditionnelles propres à un système limitées et
  regroupées après les inclusions ordinaires.

**Exemple :**

```cpp
// src/gestion/registre.cpp
#include "gestion/registre.h"

#include <stddef.h>

#include <algorithm>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "modele/eleve.h"
#include "utilitaires/texte.h"
```

**Exemple : inclusion conditionnelle.**

```cpp
#include "application.h"

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32
```
