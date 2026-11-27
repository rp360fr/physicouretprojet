# Convention C++ — Nommage et formatage

Adaptation concise des chapitres **Naming** et **Formatting** du guide C++
de Google, destinée aux projets scolaires. Les règles sont accompagnées
d'exemples, sans les longues justifications du guide original.

**Règles transversales du cours :**

- **80 caractères maximum par ligne de code.**
- **25 lignes maximum par fonction**, signature et accolades comprises.
- **2 espaces par niveau d'indentation**, jamais de tabulation.

---

# 1. Nommage (*Naming*)

## 1.1. Principes généraux

- Choisir des noms explicites et compréhensibles sans connaître tout le projet.
- Éviter les abréviations obscures ; un nom court reste acceptable si son sens
  est évident (`i` dans une boucle courte, par exemple).
- Écrire les acronymes comme des mots : `LoadHttpPage()`, pas `LoadHTTPPage()`.
- Utiliser le même style partout, sauf lorsqu'une API externe impose un autre
  nommage.

## 1.2. Tableau des conventions

| Style | À utiliser pour | Exemples |
| --- | --- | --- |
| `PascalCase` | Types (classes, structures, énumérations, concepts, alias de types), paramètres de type de templates, fonctions et méthodes sans exception | `StudentManager`, `ValueType`, `AddStudent()`, `GetGrade()`, `SetGrade()` |
| `snake_case` | Variables locales, champs de structures, espaces de noms et noms de fichiers | `student_count`, `student_name`, `school_project`, `student_manager.cpp` |
| `_snake_case` | Paramètres des fonctions et méthodes, y compris les constructeurs et lambdas | `_grade`, `_student_name` |
| `m_snake_case` | Variables membres des **classes** (pas des `struct`), hors constantes | `m_grade`, `m_student_count` |
| `kPascalCase` | Constantes à durée de vie statique, valeurs d'énumération | `kMaximumGrade`, `kCompleted` |
| `MAJUSCULES_AVEC_UNDERSCORES` | Macros | `SCHOOL_PROJECT_VERSION` |

## 1.3. Cas particuliers

- **Fichiers :** utiliser `.cpp` pour les sources et `.h` pour les en-têtes ;
  associer les fichiers portant le même nom de base. Éviter les collisions
  avec les en-têtes standard. Réserver `.inc` aux fichiers inclus textuellement.
- **Constantes locales :** le préfixe `k` est facultatif. Une valeur `const`
  recalculée à chaque appel porte normalement un nom de variable classique.
- **Paramètres de templates non typés :** les nommer selon leur rôle : variable
  ou constante.
- **Alias :** suivre le style de l'élément représenté dans le contexte de
  l'alias, pas nécessairement celui de l'élément d'origine.
- **Macros :** les éviter si une fonctionnalité C++ standard suffit ; si elles
  sont indispensables, ajouter un préfixe propre au projet.
- **Interopérabilité :** conserver le nommage d'une interface C, C++ ou Rust
  existante lorsque cela facilite la compatibilité.

## 1.4. Exemples

**Fichiers d'un même module :**

```text
student_manager.h
student_manager.cpp
student_manager_test.cpp
```

**Types, constantes, variables, attributs et méthodes :**

```cpp
namespace school_project
{

constexpr int kMaximumGrade = 20;

enum class ExamStatus
{
  kNotStarted,
  kCompleted,
};

struct StudentRecord
{
  std::string student_name;
  int         grade = 0;
};

class StudentManager
{
 public:
  void AddStudent(const StudentRecord& _student);
  int  GetStudentCount() const;

 private:
  int  m_student_count = 0;
};

}  // namespace school_project
```

**Templates et concepts (C++20) :**

```cpp
template <typename ValueType, int kCapacity>
class FixedArray
{
};

template <typename ValueType>
concept Addable = requires(ValueType _left, ValueType _right)
{
  _left + _right;
};
```

**Macro, si nécessaire :**

```cpp
#define SCHOOL_PROJECT_VERSION 1
```

---

# 2. Formatage (*Formatting*)

## 2.1. Longueur des lignes (*Line Length*)

- Limiter les lignes à **80 caractères maximum**.
- Éviter de découper inutilement un appel ou une expression qui tient sur une
  seule ligne.

**Correct :**

```cpp
int total_points = CalculateTotalPoints(student_grades, bonus_points);
```

**À reformater lorsqu'une ligne est trop longue (retour à la ligne + 4 espaces) :**

```cpp
int total_points = CalculateTotalPoints(
    student_grades, bonus_points, adjustment_points);
```

## 2.2. Espaces et tabulations (*Spaces vs. Tabs*)

- Indenter avec **2 espaces** par niveau.
- Ne jamais indenter avec des tabulations (paramétrer l'éditeur pour insérer
  2 espaces lorsque la touche Tab est utilisée).

```cpp
int SumGrades(const std::vector<int>& _grades)
{
  int total = 0;
  for (int grade : _grades)
  {
    total += grade;
  }
  return total;
}
```

## 2.3. Déclarations et définitions des fonctions (*Function Declarations and Definitions*)

- Placer le type de retour et le nom de la fonction sur la même ligne si possible.
- Ne pas mettre `{` et la signature de la fonction sur la même ligne.
- Lorsqu'un paramètre est volontairement inutilisé, omettre son nom ou
  l'indiquer en commentaire dans la définition.
- **Paramètres des fonctions préfixés par `_`** (ex. `_grade`).
- **Une fonction ne doit pas dépasser 25 lignes**, signature et accolades incluses.

**Paramètre inutilisé :**

```cpp
void HandleEvent(int /*_event_code*/)
{
  ResetState();
}
```

## 2.4. Expressions lambda (*Lambda Expressions*)

- Appliquer aux paramètres et au corps les règles des fonctions ordinaires.
- Séparer les captures par des virgules et des espaces.
- Ne pas mettre d'espace entre `&` et le nom d'une capture par référence.
- Une lambda courte peut rester sur une ligne.

```cpp
int bonus = 2;
auto add_bonus = [&bonus](int _grade) { return _grade + bonus; };

std::sort(grades.begin(), grades.end(),
          [](int _left, int _right) { return _left > _right; });
```

## 2.5. Listes d'initialisation avec accolades (*Braced Initializer List Format*)

- Mettre en forme une liste `{...}` comme les arguments d'une fonction.
- Si elle est longue, utiliser un retour à la ligne avec alignement cohérent.
- Ne pas ajouter d'espaces après `{` et avant `}`.

```cpp
std::vector<int> grades{12, 14, 16};
std::pair<int, int> coordinates{5, 10};

std::vector<std::string> student_names{
    "Alice", "Benoit", "Camille",
    "David", "Emma"};
```

## 2.6. Boucles et conditions (*Looping and Branching Statements*)

- Laisser un espace après `if`, `else`, `for`, `while`, `switch` et `do`.
- Ne pas mettre l'accolade ouvrante sur la ligne de la condition.
- Laisser un espace après chaque `;` dans une boucle `for` et autour de `:`
  dans une boucle `for` par plage.
- Indenter les étiquettes `case` de 2 espaces et leur corps de 4 espaces.

```cpp
void DisplayPassedGrades(const std::vector<int>& _grades)
{
  for (int grade : _grades)
  {
    if (grade >= 10)
    {
      PrintGrade(grade);
    }
    else
    {
      PrintFailure();
    }
  }
}
```

```cpp
void HandleExamStatus(ExamStatus _status)
{
  switch (_status)
  {
    case ExamStatus::kNotStarted:
    {
      PrepareExam();
      break;
    }
    case ExamStatus::kCompleted:
    {
      ArchiveExam();
      break;
    }
    default:
    {
      ReportStatus();
      break;
    }
  }
}
```

## 2.7. Pointeurs et références (*Pointer and Reference Expressions and Types*)

- Ne laisser aucun espace autour de `.` et `->`.
- Dans une déclaration, coller `*` et `&` au **type**, pas à la variable.
- Ne pas mettre d'espace après les opérateurs unaires `*` et `&`.
- Ne pas déclarer plusieurs pointeurs ou références sur une même ligne.

**Correct :**

```cpp
int* grade_ptr = &grade;
const std::string& student_name = student.name;
int score = *grade_ptr;
int total = student_ptr->total_points;
```

**Incorrect :**

```cpp
int *grade_ptr = &grade;
const std::string &student_name = student.name;
int* first, *second;
```

## 2.8. Expressions booléennes (*Boolean Expressions*)

- Couper les expressions trop longues en gardant un alignement cohérent.
- Placer les opérateurs `&&` et `||` en début **ou** en fin de ligne de
  manière constante ; la fin de ligne est la forme préférée ici.
- Préférer `&&`, `||`, `!`, `~` à leurs variantes textuelles (`and`, `or`, etc.).

```cpp
if (student_grade >= minimum_grade &&
    student_attendance >= minimum_attendance &&
    !student_is_absent)
{
  ValidateResult();
}
```

## 2.9. Format des classes (*Class Format*)

- Ordonner les zones : `public`, puis `protected`, puis `private`.
- Indenter les mots-clés d'accès d'**un seul espace**.
- Indenter les déclarations et définitions des membres de **2 espaces**.
- **Aligner verticalement les noms des méthodes et des variables membres** sur
  une même colonne, en ajustant les espaces après le type ou le spécificateur.
- Laisser une ligne vide avant les zones suivantes, mais aucune ligne vide
  après le mot-clé d'accès.
- Écrire la classe de base sur la même ligne si cela tient dans 80 caractères.
- Placer l'accolade ouvrante de la classe sur la ligne suivante.

```cpp
class Student : public Person
{
 public:
  explicit Student(int _grade);
  int      GetGrade() const;
  void     SetGrade(int _grade);
  void     ResetGrade();

 protected:
  void     NotifyGradeChange();

 private:
  int      m_grade = 0;
  int      m_student_id = 0;
};
```

## 2.10. Listes d'initialisation des constructeurs (*Constructor Initializer Lists*)

- Laisser `:` sur la ligne de la signature si tout tient sur une ligne.
- Sinon, couper **avant** `:` et placer celui-ci avec un retrait de 4 espaces.
- Aligner les initialisations suivantes sous le premier membre.
- Placer l'accolade ouvrante du corps sur la ligne suivante.

```cpp
Student::Student(int _grade) : m_grade(_grade)
{
}
```

```cpp
Student::Student(int _grade, int _student_id)
    : m_grade(_grade),
      m_student_id(_student_id)
{
}
```

## 2.11. Format des espaces de noms (*Namespace Formatting*)

- Placer l'accolade ouvrante du `namespace` sur la ligne suivante.
- Ne **pas** ajouter de niveau d'indentation à l'intérieur d'un `namespace`.
- Indiquer le nom de l'espace de noms à la fermeture.

```cpp
namespace school_project
{

void PrintWelcomeMessage()
{
  PrintMessage("Bienvenue !");
}

}  // namespace school_project
```

## 2.12. Espaces horizontaux (*Horizontal Whitespace*)

- Ne jamais laisser d'espaces ou de tabulations en fin de ligne.