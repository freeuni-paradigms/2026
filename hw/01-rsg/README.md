# Random Sentence Generator

Edit only `rsg.cc`. Implement this function:

```cpp
void ExpandSymbol(const std::string& symbol,
                  const Grammar& grammar,
                  RandomGenerator& random,
                  std::vector<std::string>& output);
```

If `symbol` is not a key in `grammar`, append it to `output`. Otherwise, use
`random` to select one production from the matching `Definition`, then
recursively expand that production's tokens in order. Do not create another
random-number generator.

Input grammars are valid and finite for the supplied tests. Malformed input,
undefined nonterminal references, cycles, and recursion limits are outside the
assignment contract. Parsing, command-line handling, randomness, and output
formatting are instructor-owned.

## Build and run

Set `GL_LIB_PATH` to the GL checkout, then build and run a grammar:

```sh
export GL_LIB_PATH=/absolute/path/to/gl
make rsg
./rsg data/bionic.g
```

## Tests

Build and run the tests directly from the package root:

```sh
make run_tests
./run_tests --list_tests
./run_tests
```

The same test source also supports the separate output directories used by the
grading service.

The grader has 17 equally weighted visible fixture tests. Each fixture compares
exact token vectors from three fixed seeds. The grading service builds and runs
both normal and AddressSanitizer variants. A functionally successful fixture
with an AddressSanitizer failure loses 15% of that fixture's weight through
LUnit's existing memory penalty.
