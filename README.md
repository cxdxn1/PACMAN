# PACMAN
PACMAN is a rewrite of of jprx's PacmanAttack, which utilises a hardware exploit that allows to bypass PAC (pointer authentication) on all M1 SoC's. Originally coded in Rust.

# Version support
Currently isn't functional, however macOS support will arrive as soon as those headers + main.c are done.
M1 iPads might be supported if I get PPLRW to work.
I have no idea about A14 support (M1 and A14 have the same CPU cores so it might work), it would PPLRW to work and a lot more work would need to be done to port everything to A14. I'm not making any promises though, however I will try and work on it.

Again, as of now this only works on macOS due to requiring a kext which introduces a buffer overflow exploit, however this could be achieved via KRW and/or PPLRW. I've added PPLRW (credits to opa334 for making these) to /Src/Exploits/ however keep in mind they do NOT work right now due to mapping issues and my brain not wanting to work :/.

# Credits
jprx: for PacmanAttack and creating the original PACMAN exploit.(https://github.com/jprx/PacmanAttack) 

opa334: for PPLRW (https://gist.github.com/opa334/1a2e7fd17e63e117c5bf83afb8a6dce3)
