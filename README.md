# [colat](https://sr.ht/~smlavine/colat)

Simple SDL program that displays colors.

# Usage

```
usage: ./colat [-h] [-r amt] [colors...]
-h      Prints this usage information.
-r amt  Displays `amt` randomly generated 24-bit colors in addition to
        colors provided on the command line.

Colors can be specified as 12- or 24-bit hexadecimal,
and can optionally begin with a '#' character.
If -r isn't used, at least one color must be provided.
```

# Building

Simply run `make`.

# Copyright

See [LICENSES](https://git.sr.ht/~smlavine/colat/tree/master/item/LICENSES/)
and individual files for license information.
