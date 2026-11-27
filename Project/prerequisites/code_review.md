# Code Review

La **revue de code (Code Review)** est une étape obligatoire avant toute fusion d'une branche `feature` dans `dev`. Elle doit être effectuée par au moins un autre membre de l'équipe.

Son objectif n'est pas uniquement de détecter des erreurs, mais aussi d'améliorer la qualité du code, de partager les connaissances au sein de l'équipe et de garantir une base de code cohérente et maintenable.

## 1. Responsabilités de l'auteur

Avant de demander une revue, l'auteur doit s'assurer que son travail est terminé et prêt à être intégré.

Il doit notamment :

- Vérifier que le code compile et fonctionne correctement.
- Tester les fonctionnalités développées, y compris les cas particuliers.
- Vérifier le respect des conventions de code du projet.
- Retirer les fichiers temporaires, le code mort et les traces de débogage inutiles.
- Résoudre les conflits éventuels avec `dev`.
- Fournir une **pull request** avec un titre explicite et une description des modifications apportées.

**Une revue de code ne remplace pas les vérifications que le développeur doit effectuer sur son propre travail.**

## 2. Responsabilités du reviewer

Le reviewer doit prendre le temps de comprendre les modifications proposées et leur impact sur le projet.

Il doit notamment vérifier :

- **Fonctionnement :** le code répond-il au besoin ? Les cas particuliers et les erreurs possibles sont-ils correctement traités ?
- **Lisibilité :** le code est-il compréhensible sans explications supplémentaires ? Les noms sont-ils explicites ?
- **Architecture :** les responsabilités sont-elles correctement réparties ? Les modifications respectent-elles l'organisation du projet ?
- **Maintenabilité :** le code est-il suffisamment simple pour être corrigé, réutilisé et faire évoluer les fonctionnalités ?
- **Conventions :** les règles de nommage, de formatage et les pratiques imposées sont-elles respectées ?
- **Duplication :** des parties de code similaires pourraient-elles être regroupées ?
- **Tests :** les vérifications effectuées sont-elles suffisantes pour valider le comportement attendu ?

Le reviewer ne doit pas se limiter à l'apparence du code. **Un code correctement formaté n'est pas nécessairement un code de qualité.**

## 3. Commentaires et échanges

Les commentaires doivent être **précis, constructifs et justifiés**.

Un commentaire doit expliquer le problème constaté et, lorsque cela est possible, proposer une piste d'amélioration.

On distingue deux types de remarques :

- **Bloquante :** problème devant obligatoirement être corrigé avant la fusion (bug, non-respect d'une convention obligatoire, comportement incorrect, etc.).
- **Suggestion :** amélioration possible qui ne conditionne pas nécessairement la validation de la pull request.

Les préférences personnelles ne doivent pas être imposées lorsqu'elles ne reposent sur aucune règle du projet ou justification technique.

L'auteur doit prendre connaissance de chaque remarque, y répondre et effectuer les corrections nécessaires. Il peut également argumenter lorsqu'il estime qu'une modification proposée n'est pas pertinente.

**La revue doit être un échange technique, et non une confrontation entre développeurs.**

## 4. Validation de la revue

Une pull request ne peut être fusionnée que lorsque :

- Les remarques bloquantes ont été corrigées.
- Les éventuelles discussions non résolues ont été traitées.
- Le code a été vérifié après les dernières modifications.
- Au moins un autre membre de l'équipe a explicitement approuvé la pull request.

Lorsqu'une correction importante est apportée pendant la revue, le reviewer doit vérifier les nouvelles modifications avant de donner son approbation.

Une fois la revue validée, la branche `feature` peut être fusionnée dans `dev`, puis supprimée conformément au Git Flow du projet.

## 5. Esprit de la revue

Une revue de code n'est pas une simple formalité administrative.

Chaque membre de l'équipe doit être capable d'expliquer les modifications qu'il approuve. Valider une pull request sans l'avoir réellement examinée va à l'encontre de l'objectif de cette démarche.

La revue est également une occasion d'apprentissage : les auteurs comme les reviewers doivent accepter la critique constructive et être capables de justifier leurs choix techniques.
