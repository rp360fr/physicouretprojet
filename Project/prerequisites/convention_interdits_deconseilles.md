# C++ — Pratiques interdites ou déconseillées

**Légende :** **[I]** Interdit · **[D]** Déconseillé

## 1. Formatage

- **[I]** Ne pas dépasser **25 lignes** par fonction.
- **[I]** Ne pas dépasser **80 caractères** par ligne.

## 2. Portée, espaces de noms et variables

- **[I]** Ne pas écrire `using namespace ...;`.
- **[I]** Ne pas utiliser d'espaces de noms `inline`.
- **[I]** Ne pas déclarer de fonctions dans l'espace de noms global ; préférer les espaces de noms nommés.
- **[I]** Déclarer les variables locales dans la portée la plus réduite possible et les initialiser dès leur déclaration.
- **[I]** Ne pas utiliser d'objets globaux ou `static` dont le destructeur n'est pas trivial (par exemple, des objets `std::string`, `std::vector` ou `std::map` statiques).

## 3. Classes, structures et héritage

- **[I]** Utiliser uniquement l'héritage `public` ; remplacer l'héritage privé par la composition.
- **[I]** Ne pas exposer les attributs des classes en `public`.
- **[I]** Utiliser une `struct` avec des champs nommés plutôt qu'un `std::pair` ou un `std::tuple` lorsque les éléments ont un sens métier précis.
- **[I]** Réserver les `struct` aux objets passifs transportant des données ; employer `class` pour les types dotés d'invariants ou de comportements.

---

- **[D]** Éviter les méthodes `Init()` qui laissent temporairement un objet dans un état partiellement initialisé.
- **[D]** Éviter l'héritage d'implémentation excessif et, plus encore, l'héritage multiple de plusieurs implémentations.
- **[D]** Ne pas abuser de `friend`.

## 4. Fonctions

- **[I]** Ne pas retourner de pointeur brut lorsque le résultat ne peut jamais être nul ; préférer une valeur ou une référence selon le cas.
- **[I]** Ne pas conserver, au-delà de l'appel, une référence passée en paramètre à une fonction.

---

- **[D]** Éviter les paramètres de sortie ; préférer la valeur de retour.
- **[D]** Préférer les surcharges aux arguments par défaut lorsqu'elles sont plus lisibles.
- **[D]** Éviter la syntaxe `auto Fonction() -> Type` lorsque le type de retour peut facilement être écrit avant le nom.

## 5. Fonctionnalités C++ et bibliothèque standard

### 5.1. Mémoire, références, exceptions et conversions

- **[I]** Ne pas utiliser de pointeurs intelligents (*smart pointers*).
- **[I]** Ne pas utiliser les exceptions C++ (`throw`, `try`, `catch`) ni les fonctionnalités associées (`std::exception_ptr`, etc.). Cette règle s'applique à tous les projets de jeux vidéo.
- **[I]** Ne pas utiliser de RTTI (`typeid`, `dynamic_cast`) en dehors des tests.

---

- **[D]** Éviter les conversions de style C : `(Type)valeur` ; préférer les conversions explicites C++ appropriées.
- **[D]** Limiter fortement l'utilisation de `reinterpret_cast` et de `const_cast` aux cas où leurs conséquences sont parfaitement comprises.

### 5.2. Expressions, types et entrées/sorties

- **[I]** Utiliser `const` lorsque l'immuabilité est garantie (paramètres par référence ou pointeur, méthodes constantes).
- **[I]** Utiliser `nullptr` plutôt que `NULL` ou `0` pour les pointeurs, et `'\0'` plutôt que `0` pour le caractère nul.

---

- **[D]** Préférer `++i` et `--i` à `i++` et `i--` lorsque l'ancienne valeur n'est pas utilisée.
- **[D]** Ne pas complexifier une fonction uniquement pour lui ajouter `constexpr` ; ne pas employer `constexpr` ou `consteval` pour imposer l'inlining.
- **[D]** Éviter `short`, `long` et `long long` ; préférer `int` ou des types de taille explicite (`int64_t`, etc.) selon le besoin.
- **[D]** Éviter les entiers non signés (`unsigned`, `uint32_t`, etc.) pour représenter un nombre simplement non négatif ; ils restent pertinents pour les masques de bits et l'arithmétique modulaire.
- **[D]** Préférer `sizeof(variable)` à `sizeof(Type)` lorsqu'on mesure la taille d'une variable précise.

### 5.3. Préprocesseur et déduction de type

- **[I]** Pour les initialisations désignées (`.champ = valeur`), respecter la syntaxe et l'ordre des champs imposés par C++20.
- **[I]** Ne pas employer `auto` uniquement pour éviter d'écrire un type ; l'utiliser si le type est évident ou si la déduction rend le code plus sûr.

---

- **[D]** Éviter les macros en général, surtout dans les en-têtes ; préférer les fonctions, `constexpr`, les constantes et les énumérations.
- **[D]** Éviter `decltype(auto)` lorsqu'une construction plus simple suffit.

### 5.4. Lambdas et programmation générique

- **[I]** Ne pas redéfinir des concepts ou des traits existants et ne pas créer de concepts dont les exigences ne sont pas vérifiables par le compilateur.
- **[I]** Ne pas utiliser de captures implicites (`[&]`, `[=]`) lorsqu'une lambda peut survivre à la portée des variables capturées ; préférer des captures explicites et sûres.
- **[I]** Ne pas capturer implicitement `this`, surtout avec `[=]`.
- **[I]** Ne pas écrire de lambdas longues, imbriquées ou difficiles à lire.
- **[I]** Utiliser `using` plutôt que `typedef` pour les alias de types.

---

- **[D]** Employer les concepts C++20 avec parcimonie ; éviter de définir des concepts publics spécifiques à une bibliothèque.
- **[D]** Préférer `requires(...)` à `template<Concept T>` ou à `std::enable_if` lorsqu'une contrainte est réellement nécessaire.
