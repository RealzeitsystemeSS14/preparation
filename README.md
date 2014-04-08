#Raspberry Schlafzeitmessung

## Installation

* raspbian wie [hier] (http://www.raspberrypi.org/forums/viewtopic.php?f=66&t=50310) beschrieben installieren
* dann folgende Programme auf dem RPI installieren

```
apt-get update
apt-get install killall
apt-get install gnuplot
apt-get install sshpass
```

## Kompilieren

* Git-Repository auf homer auschecken

```
git clone url
```

* in das __src/__ Verzeichnis wechseln und make aufrufen

```
make
```

* __ANMERKUNG:__ standardmäßig benutzt die Makefile die __raspian toolchain__ auf homer

## Ausführung

* in das Repo-Verzeichnis wechseln
* __copyToPi__ in Editor öffnen und Target (IP-Adresse), Passwort, User und Zielverzeichnis entsprechend anpassen
* copyToPi ausführen

```
./copyToPi
```
* __ALTERNATIVE:__ es kann auch einfach manuell der __scp__ Befehl verwendet werden, um die __Binaries und Skripte__ zu transferieren
* nun per SSH auf das RPI verbinden
* vor dem Starten der Programme muss __S98initLED_TAST__ ausgeführt werden, um die __Sys-GPIO-Schnittstelle zu initialisieren__

```
./S98initLED_TAST
```

* die Programme können wie in der Aufgabenstellung vorgesehen benutzt werden
* das Skript __start_all__ startet die SleepMessung in verschiedenen Varianten (mit/ohne RT-Prio; mit / ohne Heavy Load)
* dabei werden die __gnuplot files__ mit unterschiedlichen Namen gespeichert:
  * __result.eps__ = ohne Load + ohne RT-Prio
  * __result_rt.eps__ = ohne Load + mit RT-Prio
  * __result_heavyLoad.eps__ = mit Load + ohne RT-Prio
  * __result_heavyLoad_rt.eps__ = mit Load + mit RT-Prio
* die Programme __switch__ und __led5__ benötigen denselben Aufbau aus der Aufgabenstellung (gleiche GPIO-Pins)

## Interpretation

Die Diagramme zeigen die maximale Abweichung zwischen der eingestellten und der gemessenen Zeit (y-Achse) und die eingestellte Sollschlafzeit (x-Achse).

Zu sehen ist, dass der Prozess selbst unter Last eine konstante und geringe Abweichung hat, wenn er mit RT-Priorität betrieben wird. Ohne RT-Priorität gibt es immer wieder größere Abweichungen vorallem im Betrieb mit viel Systemlast.

Der Prozess wird mit RT-Priorität im System also deutlich bevorzugt!

