# Tests unitaires — GoogleTest et TDD

Les tests unitaires vérifient automatiquement le comportement d'une fonction ou
 d'un composant, indépendamment du lancement complet de l'application.

**Le développement doit suivre la méthode TDD (*Test-Driven Development*) :
le test est écrit avant l'implémentation de la fonctionnalité.** Le test sert
ensuite de point d'entrée direct pour exécuter et déboguer le code concerné.

## 1. Développement guidé par les tests (TDD)

Pour chaque nouveau comportement, appliquer le cycle **Red → Green → Refactor** :

1. **Préparer l'interface :** créer au besoin les fichiers, classes et signatures
   de fonctions, sans implémenter le comportement. Le projet doit pouvoir
   compiler et les tests doivent pouvoir s'exécuter.
2. **Red — Écrire le test :** définir le résultat attendu avec GoogleTest, puis
   lancer le test. **Il doit échouer pour la bonne raison** : le comportement
   demandé n'est pas encore implémenté, et non parce que le projet ne compile pas.
3. **Green — Développer :** écrire le minimum de code nécessaire pour faire
   réussir le test.
4. **Refactor — Améliorer :** nettoyer, simplifier ou réorganiser le code sans
   modifier le comportement. Relancer les tests après chaque changement.
5. **Recommencer :** ajouter un nouveau test pour le comportement ou le cas
   particulier suivant.

**Ne pas écrire toute une fonctionnalité avant d'en créer les tests.**
Un test qui réussit immédiatement, avant toute implémentation, ne démontre pas
qu'il protège le comportement que l'on souhaite développer : vérifier alors
que le cas testé est pertinent.

## 2. Exemple complet : développer une fonctionnalité à partir d'un test

**Besoin :** un compteur de points doit permettre d'ajouter des points positifs.
Les valeurs négatives seront traitées dans une seconde itération.

### 2.1. Préparer la classe, sans développer la fonctionnalité

`score_counter.h` :

```cpp
#pragma once

class ScoreCounter
{
 public:
  void AddPoints(int _points);
  int  GetTotal() const;

 private:
  int  m_total = 0;
};
```

`score_counter.cpp` (implémentation provisoire) :

```cpp
#include "score_counter.h"

void ScoreCounter::AddPoints(int _points)
{
  // Comportement non implémenté : premier test attendu en échec.
}

int ScoreCounter::GetTotal() const
{
  return 0;
}
```

Ce squelette permet de **compiler et d'exécuter le test**, même si la
fonctionnalité n'existe pas encore.

### 2.2. Red — Écrire et exécuter le premier test

`score_counter_test.cpp` :

```cpp
#include <gtest/gtest.h>

#include "score_counter.h"

TEST(ScoreCounterTest, AddsPositivePoints)
{
  ScoreCounter counter;

  counter.AddPoints(5);

  EXPECT_EQ(5, counter.GetTotal());
}
```

Le test doit **échouer**, car `GetTotal()` renvoie encore `0` au lieu de `5`.
L'échec confirme que le test détecte l'absence du comportement demandé.

### 2.3. Green — Écrire le minimum de code

Remplacer les implémentations provisoires dans `score_counter.cpp` par :

```cpp
void ScoreCounter::AddPoints(int _points)
{
  m_total += _points;
}

int ScoreCounter::GetTotal() const
{
  return m_total;
}
```

Relancer `ScoreCounterTest.AddsPositivePoints` : **il doit réussir**.

### 2.4. Nouveau cycle — Tester un cas particulier

**Nouveau besoin :** ignorer les ajouts de points négatifs.
Écrire ce test **avant de modifier** `AddPoints()` :

```cpp
TEST(ScoreCounterTest, IgnoresNegativePoints)
{
  ScoreCounter counter;

  counter.AddPoints(5);
  counter.AddPoints(-3);

  EXPECT_EQ(5, counter.GetTotal());
}
```

Avec l'implémentation précédente, le résultat vaut `2` : le test échoue
(**Red**). Corriger alors la fonction (**Green**) :

```cpp
void ScoreCounter::AddPoints(int _points)
{
  if (_points > 0)
  {
    m_total += _points;
  }
}
```

Relancer **tous** les tests, puis refactoriser si nécessaire. On peut ensuite
ajouter d'autres cas, comme un ajout nul ou plusieurs ajouts successifs.

## 3. GoogleTest : les outils essentiels

- `TEST(Suite, Comportement)` : définit un test indépendant.
- `TEST_F(Fixture, Comportement)` : définit un test partageant une préparation
  commune avec d'autres tests de la même fixture. GoogleTest crée une nouvelle
  instance de la fixture pour chaque test.
- `EXPECT_EQ(attendu, obtenu)` : signale un échec mais laisse le test continuer.
- `ASSERT_EQ(attendu, obtenu)` : arrête le test courant si l'assertion échoue.
- `EXPECT_TRUE(...)`, `EXPECT_FALSE(...)` : vérifient une condition booléenne.

Exemple avec une **fixture**, lorsque plusieurs tests utilisent la même
initialisation :

```cpp
#include <gtest/gtest.h>

#include "score_counter.h"

class ScoreCounterFixture : public ::testing::Test
{
 protected:
  ScoreCounter m_counter;
};

TEST_F(ScoreCounterFixture, StartsAtZero)
{
  EXPECT_EQ(0, m_counter.GetTotal());
}

TEST_F(ScoreCounterFixture, AddsPointsTwice)
{
  m_counter.AddPoints(2);
  m_counter.AddPoints(3);

  EXPECT_EQ(5, m_counter.GetTotal());
}
```

Une fixture est utile lorsque la préparation est répétée ; elle n'est pas
obligatoire pour les tests simples.

## 4. Déboguer directement le comportement testé

Chaque test constitue un **point d'entrée ciblé** : il prépare les données,
appelle la fonction et vérifie son résultat, sans passer par les menus, les
scènes ou toute autre interface de l'application.

Exécuter un test précis dans le binaire GoogleTest du projet :

```bash
./build/unit_tests --gtest_filter=ScoreCounterTest.AddsPositivePoints
```

Pour entrer directement dans le débogueur sous Linux, avec GDB :

```bash
gdb --args ./build/unit_tests --gtest_filter=ScoreCounterTest.AddsPositivePoints
```

Placer un point d'arrêt dans `ScoreCounter::AddPoints`, puis lancer le programme.
Dans un IDE, sélectionner le test et utiliser **Debug** plutôt que **Run**.
Le binaire de test doit avoir été compilé avec les informations de débogage.

## 5. Qualité et périmètre des tests

Les tests doivent :

- Vérifier un **comportement attendu**, pas reproduire l'implémentation.
- Couvrir, lorsque c'est pertinent, les cas nominaux, limites et invalides.
- Être indépendants, reproductibles et exécutables dans n'importe quel ordre.
- Utiliser des noms indiquant ce qui est vérifié.
- Limiter les dépendances au rendu graphique, au réseau, aux fichiers et aux
  interactions manuelles ; utiliser des dépendances simulées si nécessaire.
- Rester simples et respecter les conventions C++ du projet.

Il n'est pas nécessaire de tester chaque getter trivial. En revanche, toute
correction de bug doit, lorsque c'est possible, commencer par un test qui
**reproduit le bug**, puis réussir après sa correction.

## 6. Tests et pull requests

Avant d'ouvrir une pull request :

- Exécuter les tests de la fonctionnalité et l'ensemble des tests du projet.
- Vérifier qu'aucun test existant ne régresse.
- Fournir les tests correspondant aux nouveaux comportements ou aux bugs
  corrigés.
- Ne jamais supprimer ou désactiver un test uniquement parce qu'il échoue.

Lors de la **Code Review**, le reviewer vérifie que les tests sont pertinents,
qu'ils exercent réellement la fonctionnalité et que leur résultat est fiable.
Une pull request introduisant une régression connue ne doit pas être validée.

---

**Documentation GoogleTest :**
[Premiers pas](https://google.github.io/googletest/primer.html) ·
[Options d'exécution et filtres](https://google.github.io/googletest/advanced.html)