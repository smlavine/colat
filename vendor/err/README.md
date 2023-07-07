[![err :)](logo.jpg)](https://sr.ht/~smlavine/err)

err is a small error-printing library. It can be used by projects
written in any major version of C or C++.

See [the website](https://err.smlavine.com) for API documentation and
examples.

# Using err

The easiest way to use err is to copy err.c and err.h into your project.
You can download the latest version of err from

- <https://git.sr.ht/~smlavine/err/blob/2.0.0/err.c>
- <https://git.sr.ht/~smlavine/err/blob/2.0.0/err.h>

Alternatively, you may wish to import err as a git subtree:

```
git subtree --squash -P err/ add https://git.sr.ht/~smlavine/err 2.0.0
```

`#include "err.h"` in files where you want to use err functions.

Be sure to assign a value to `program_invocation_name` before calling
any err functions.

## Generating documentation

err uses Doxygen to generate documentation. To compile the documentation
into HTML, run `doxygen`. The generated files will be put
into the `site/` directory.

# Copyright

Copyright (C) 2021-2022 Sebastian LaVine <mail@smlavine.com>

Licensed under the MPLv2. See [LICENSE][license].

[license]: https://git.sr.ht/~smlavine/err/tree/master/item/LICENSE
