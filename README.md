# Porting UEFI to a new architecture

So it turns out that blogging about something after the fact is pretty
tough. I really wanted to blog about my PoC port of UEFI to the
OpenPower ecosystem, but it's incredibly difficult to go back and try
to systematize something that's been a few years back.
 
So let's try this again. This time, our victim will be a G4 12"
PowerBook6,8 with a 7447A. That's a 32-bit PowerPC. Now, I'll go in
small steps and document *everything*. For added fun, we'll begin
porting on the target itself, at least until that gets too tedious.
Also, I've a few OldWorld machines, a spare G4 12" for parts and
a G5, so hopefully this odyssey won't be interrupted by old
and failing hardware ;-).

Keep in mind that each part is checked in along with the source code,
so look at the entire commit. Each blog post will focus on the most
important details.

* [Part 1 - preparing the environment and build tools](PortingHowTo_p1.md)
* [Part 2 - making the build system aware of the new architecture](PortingHowTo_p2.md)
* [Part 3 - where we almost build the dummy application](PortingHowTo_p3.md)
* [Part 4 - where we try to generate an EFI binary](PortingHowTo_p4.md)