# WIZZ Mania — MSN est de retour

Application de chat temps réel (TCP/IP) en C++ avec un **serveur multi-clients** et un **client Qt** façon MSN (contacts, Wizz, messages privés).

## Contenu du repo

- **`commun/`** : bibliothèque commune (TCP + `Message`).
- **`serveur/`** : serveur multi-clients (threads) + auth (register/login) + contacts + privé.
- **`client/`** : client console historique (broadcast).
- **`gui/`** : client Qt (`wizz_gui`) avec login/register, contacts, conversations, Wizz, emojis.
- **`tests/`** : tests Catch2 (unit + intégration).
- **`tools/`** : outils CLI (ex: `auth_tester`).

## Protocole (résumé)

Encodage: **1 message = 1 ligne terminée par `\n`**.

Format historique:

- `TYPE|AUTEUR|CONTENU`

Format étendu (ex: privé):

- `PRIVE|AUTEUR|CIBLE|CONTENU`

Types principaux:

- **Broadcast**: `MESSAGE`, `WIZZ`, `CONNEXION`, `DECONNEXION`
- **Auth**: `REGISTER`, `LOGIN`, `AUTH_OK`, `AUTH_FAIL`
- **Contacts**: `CONTACTS` (payload `name=1;other=0;...`)
- **Privé**: `PRIVE`

## Prérequis (Windows)

- **CMake** (déjà ok si vous avez `cmake`).
- **Qt 6 MinGW** (ex: Qt Creator installe souvent dans `C:\Qt\6.10.1\mingw_64`).
- **MinGW du kit Qt** (ex: `C:\Qt\Tools\mingw1310_64`).

Important: pour éviter les incompatibilités ABI, compilez le projet avec **le MinGW de Qt**.

## Build (recommandé: Qt MinGW) — `build-qt/`

Dans PowerShell:

```powershell
cd "C:\Users\Abricot\Desktop\Wizz_mania"

cmake -S . -B build-qt -G "MinGW Makefiles" `
  -DWIZZMANIA_BUILD_GUI=ON `
  -DWIZZMANIA_BUILD_TOOLS=ON `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/mingw_64" `
  -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" `
  -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe"

cmake --build build-qt -j 8
```

Les binaires sortent dans:

- `build-qt\bin\serveur.exe`
- `build-qt\bin\wizz_gui.exe`
- `build-qt\bin\auth_tester.exe`
- `build-qt\bin\tests_wizzmania.exe`

## Lancer (serveur + GUI)

### 1) Lancer le serveur

```bat
cd C:\Users\Abricot\Desktop\Wizz_mania
.\build-qt\bin\serveur.exe
```

Le serveur crée un fichier `users.db` **dans le dossier courant**.

### 2) Lancer le client Qt

Si Qt n’est pas dans le PATH global, dans PowerShell:

```powershell
$env:PATH="C:\Qt\6.10.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;$env:PATH"
.\build-qt\bin\wizz_gui.exe
```

Dans le GUI:

- Register puis Login
- Contacts online/offline + conversations
- Wizz (shake/flash/toast) + emojis

## Tests automatisés (Catch2)

Pour **démontrer** les tests:

```bat
cd C:\Users\Abricot\Desktop\Wizz_mania
.\build-qt\bin\tests_wizzmania.exe
```

Ce binaire lance:

- unit tests `Message`
- unit tests `UserStore`
- test d’intégration: démarre `serveur.exe` puis lance `auth_tester.exe` (register/login/privé)

## Outil CLI (auth_tester)

Exemples:

```bat
cd C:\Users\Abricot\Desktop\Wizz_mania
.\build-qt\bin\auth_tester.exe register Alice pasword(...)
.\build-qt\bin\auth_tester.exe login Alice pasword(...) --listen-ms 1500
.\build-qt\bin\auth_tester.exe login Alice pasword(...) --send-private Bob "Salut Bob!" --listen-ms 1500
```

## Packaging portable (windeployqt)

Créer un dossier portable (copie des DLL/plugins Qt):

```powershell
cd "C:\Users\Abricot\Desktop\Wizz_mania"
$outDir = ".\out\wizz_gui_portable"
if (Test-Path $outDir) { Remove-Item -Recurse -Force $outDir }
New-Item -ItemType Directory -Path $outDir | Out-Null
Copy-Item -Force .\build-qt\bin\wizz_gui.exe $outDir

$env:PATH="C:\Qt\6.10.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;$env:PATH"
& "C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe" --compiler-runtime --no-translations --dir $outDir "$outDir\wizz_gui.exe"
Compress-Archive -Path "$outDir\*" -DestinationPath ".\out\wizz_gui_portable.zip" -Force
```

Résultat:

- `out\wizz_gui_portable\` (dossier exécutable)
- `out\wizz_gui_portable.zip` (partage facile)
