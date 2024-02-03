A utility to deterministicly generate a askii password from a potentally longer passphrase (so you can use a master passphrase for multiple services, with each having a different password)

Entropy (and security) is not tested!  DO NOT USE!

to build:
clone
make oqs
make release

for a web build:
clone
make oqs_em
make webcpp_em
#release_em is theoretically better (has optimizations, no debug symbols), but isn't working right now.
make all_em

Accessess a web build here: https://0x0015.github.io/passkeygen
