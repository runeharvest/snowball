This is a fork of [Ryzom Core](https://github.com/ryzom/ryzomcore), that uses [AGPLv3](./LICENSE).

The majority of the code here is also under the same license of AGPLv3, if not all, as per notes below note.


# Snowballs2

Snowballs2 serves as a reference implementation and starting point for developing multiplayer games or networked applications

Snowballs2 is a sample multiplayer client-server application built on the NeL (Nevrax Library) framework. It demonstrates core concepts such as networking, client-server communication, basic game logic, and modular service architecture.

## Structure

- **client/**: Contains the client application source code, configuration, and resources. Implements rendering, user interface, networking, and game logic.
- **server/**: Contains modular server services (chat, collision, frontend, position, empty) with separate configurations and source code for each service.
- **bin/**: Configuration files and scripts for running and managing the various services and the client.
- **resources/**: Art assets and additional configuration files.

## Features

- Modular server architecture with separate services for chat, collision, position, frontend, and empty services.
- Multiplayer networking and communication between client and server.
- Basic game mechanics, including entity management, physics, and user interface.
- Example configuration and scripts for launching and testing the application.



# Original README


1. What's this document about?
   ---------------------------

This document contains useful information about SNOWBALLS, NeL, Nevrax,
technical requirements, and caveats about compilation and installation.
It also includes a short "user manual" for the programs provided. It is
highly recommended reading.


2. What are NeL and Nevrax
   -----------------------

NeL is a free software framework developped by Nevrax Ltd, for the purpose of
building internet multiuser entertainment.

Nevrax is a company dedicated to Free Software, which is why their tools are
covered by the GPL. See the attached license file. They built NeL to create
their massively multiplayer online role playing game called The Saga of Ryzom.

You should visit our website at http://www.opennel.org/ for more information.


3. What's in the package?
   ----------------------

The package contains:

   - The latest source code for the NeL client and server frameworks
   - The source code and data for a sample NeL game called 'SNOWBALLS'

If you have a source distribution, you need to compile the source to
produce the various executables. If you have a binary distribution,
you have a pre-built exe client for Snowballs, but no source.

The source code is provided as is, and reflects the state of the NeL
framework at the release date. We have done our best to insure that the
snapshot you are using now compiles and works correctly. However, we
only test on a limited range of configurations, and the vagaries of
OS and hardware means we cannot guarantee it compiles and executes
correctly on your system at the time.

If you are using SVN, you can keep in synch with the NeL Development Team
as we have included the required SVN information. An update will give you
current source. We do not guarantee that the source in SVN will compile
and work correctly at all time as it includes the latest work in progress.


4. Caveat
   ------

The sample client, server and data files provided do not accurately reflect
our game currently in production. We do intend penguins and gnus to appear
in the final game, but most of the gameplay you will see does not reflect
what we will implement later.


5. Compiling NeL and SNOWBALLS
   ---------------------------

TODO: update text.
You will need a C++ compiler to properly compile and execute the NeL
framework. The program compiles reasonably under a Unix/GCC environment,
but it has been chiefly tested under a Linux environment.

TODO: update text.
The server has been compiled and executed on a Windows machine, but it
is highly recommended that you run it on a separate GNU/Linux machine. The
client has been compiled and executed on a GNU/Linux machine, but will
probably not work on other unix compatible systems (yet).

The client and server require an operating TCP/IP stack to communicate.

The data files for the client and server should be in the ./data child
directories of the directory in which the executables are located.

See the INSTALL file for the requirements and installation instructions.


6. Running SNOWBALLS client
   ------------------------

It is highly recommended that you run the client on a PC with plenty of
main memory (128Megs+) and a 3D card that supports T&L in hardware and
has 32Mb of memory. Lesser configurations will probably give you very
poor framerates.

TODO: rewrite completely, incorrect.
You just have to execute nel_launcher.exe (and never snowballs.exe directly).
The application will prompt you a login and password. If you enter
a user name that has never been used before on the server that you are
connected to then a new account will be created with the password that
you enter at the 'PASSWORD' prompt. You'll need to remeber this password
as you'll need each time you login in the future. Use only alphanumeric
value for login and password, don't use space and other strange characters.
WARNING: Password are not crypted so you should use 'dummy' password.

TODO: rewrite completely, incorrect.
After you have to select a shard if available, just click on the shard name.
It patch if necessary and automatically launch snowballs game, just wait.

TODO: rewrite completely, incorrect and extremely boring.
You're now a penguin on an artic snowfield. You have snowballs which you
can throw at other people. When you throw a snowball, you have a few
seconds during which you gather more snow before you can throw the next
one. If you hit someone, your score increases, and the snowed opponent
becomes immune to further snowing (he flashes on the screen) for 5s. He
cannot retaliate while invulnerable.

Good luck.


7. The controls in SNOWBALLS
   -------------------------

Use the mouse to look around.
Use arrow keys to move forward, backward, strafe left and strafe right.

F3: switch on/off the wireframe mode
F4: clear the chat window
F5: switch on/off the chat text
F6: switch on/off the radar
F7: zoom out the radar
F8: zoom in the radar
F9: capture or uncapture the mouse
F10: switch betweeon online and offline
F11: reset player position
F12: take a screenshot
SHIFT-ESC: quit
Left Mouse Button: throw a snowball
Type text followed by ENTER to broadcast messages


8. Contacting us
   -------------

For comments and problems relating to SNOWBALLS or NeL
you can go to the forums at http://www.opennel.org/jforum/forums/list.page
or use the issue tracker at http://www.opennel.org/jira/secure/Dashboard.jspa



## Copyright and contributions

All original source code is copyright (C) 2001-2022 Winch Gate Property Limited. The commercial game's official Git repositories are hosted on GitLab at https://gitlab.com/ryzom.

The NeL Sound driver for XAudio2 is copyright (C) 2008-2014 Jan BOON <jan.boon@kaetemi.be>.

Other community contributions and modifications are copyright their respective authors. Consult the OpenNeL SVN and the Ryzom Core Git repository histories for more details.

The source code in this repository is licensed under the AGPLv3, unless specified otherwise. Likewise, by committing to this repository, you agree to license your modifications under the AGPLv3. You retain all copyright over your work (\*). As such, you have the option to additionally dual-license your fully-owned contributions under any other license. Configuration files are implied to be public domain samples, as they may contain sensitive data in a production environment.

(\*) If, and only if, you have signed a CTA (part of the NDA, copyright and ownership - your contract may vary) with Winch Gate Ltd., your contributions, along with any other creations related to Ryzom, fall under that contract and will be copyright Winch Gate Ltd. instead (\*\*). As per your contract, your contributions are licensed back to you under the AGPLv3. You retain the moral rights to your work. These are non-transferable rights, which permit you to assert your authorship and be credited for your contributions. You should exercise this right.

(\*\*) If you live in a legislation where contractual copyright assignments without employment or commercial transaction are unenforceable (e.g. Belgium), and you are not being paid by Winch Gate Ltd. for your current work, the CTA is null and void, and you retain full copyright ownership over your contributions. Consult a lawyer to assert your rights. Don't give them away.

To comply with the AGPLv3 license, section 5(a), names of the contributors who own the copyrights to modifications will be added to the copyright notice in source files on a periodic basis. You may follow the existing format if you wish to do this yourself.

The removal of any valid copyright notice is a violation of the AGPLv3 license.
