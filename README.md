# BA-ReversiML-JosefLanzl-202122

Dies ist das GitLab-Projekt zur Bachelorarbeit:
**Implementierung und Vergleich von Algorithmen basierend auf Monte-Carlo-Tree-Search und AlphaZero für das strategische Brettspiel Reversi**

Das Ziel ist es, einen Client zu implementieren, der verschiedene Algorithmen ausführt, ausgewählte Parameter speichert und anschließend auswertet.
Hierfür sind Monte-Carlo-Tree-Search und MiniMax Hybride verwendet worden.
Zusätzlich gibt es einen Client, der mithilfe eines neuronalen Netzes trainiert.

**Struktur:**

- Abschlussarbeit: beinhaltet die Bachelorarbeit als LaTeX-Source-Code
- Binaries: beinhaltet Skripte, die für das Ausführen und Auswerten der Versuche zuständig sind
- src: beinhaltet die Source-Code des Programms (C++, Python, Cython) und die dazugehörigen Unit Tests

**Systemvoraussetzungen:**

- Betriebssystem: Ubuntu (Implementierung ist in Version 20.04. erfolgt)
- Python3
- Cython3
- Tensorflow
- numpy
- pandas
- jq

**Anleitung:**

1. Systemvoraussetzungen müssen erfüllt sein. 
2. Die Skripte zum Ausführen der Dateien befinden sich im Ordner Binaries.
3. run.sh: Ausführen der MCTS-Hybride. Für die Konfiguration ist hierbei die Datei config.json, die sich im selben Ordner befindet, anzupassen.
4. cpyRun.sh: Starten des Trainings des neuronalen Netzs. Die Konfiguration erfolgt hierbei durch die Datei config.py. Diese befindet sich in /src/Reversi/cpy/ .
5. Tabellen im CSV-Format werden während der Spiele erstellt und regelmäßig erweitert. Das schlussendliche automatisierte Auswerten ist bislang nur für run.sh umgesetzt.
6. Um die bisher besten Modelle für das neuronale Netz zu verwendet, müssen die gespeicherten Modelle in /Binaries/models kopiert werden. Hierbei sind nur ResNets zugelassen. Eine Anpassung der Schichtenanzahl erfolgt in /src/Reversi/cpy/config.py .
