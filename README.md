# SimChemistryForWindows

Archived source, documents and executable for SimChemistry for Windows

(c) Copyright 1996-2021 Charlie Wartnaby

# Description

SimChemistry for Windows is a project dating back to the 1990s (with C++ style to match). The
idea was that it allowed users to set up and then run live molecular dynamics simulations on
screen, to better understand physical and chemical principles that emerge from
microscopic behaviour.

Although the code is pretty much obsolete, it has some heavily commented algorithms which may
still be of interest. It features both event-based simulation (e.g. when molecules are modelled
as hard 2D discs which collide instantaneously), and soft force-based motion.

As well as molecules, it includes pistons (which squeeze molecules), walls to contain them,
and monitor regions to measure properties. You can also add annotations.

The algorithms include:
* Managing a discrete-time simulation with both soft (forces) and hard (collisions) interactions
* Modified Lennard-Jones interaction between molecules
* Elastic collision interactions between molecules, walls and pistons
* Ineleastic collisions between molecules and walls (to set temperature)
* Reactive collisions between molecules above an activation energy, conserving momentum, respecting a formation energy
* Interactions to allow monitoring regions to measure and control properties

Finally, scripts can be recorded and replayed to provide repeatable demonstrations, for
teaching.

## System Requirements
SimChemistry for Windows 2.2 should run under any 32-bit version of Windows (95 or higher).

## General

This project was previously available at http://www.simchemistry.co.uk, but I may not renew
that domain again, hence it is now here. However, it is not maintained.

You are welcome to use the source code in your own programs. See code/LICENCE_README.txt
for details.

Finally, I'm embarrassed to say that the code is indented mainly with tabs (as was the
Microsoft fashion when it started) but in places with spaces too, which looks horrible
now it is here on GitHub. Ah, the wisdom of age...
