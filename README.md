# cnippet

![Demo](https://github.com/xyl1t/cdn/raw/refs/heads/main/cnippet/cnippet_demo.mp4)

This is one half of a hack for getting around the fact that
[Helix](https://helix-editor.com/) has a lack of supporting snippets.

How it works:

You pass a snippet (as an argument or pipe it) to the program and it outputs
the value for that snippet:

```
$ ./cnippet a
<a href="_1_">___</a>
```

## Snippet syntax

There is an example snippet file available in this repository (`snippets.txt`),
but the syntax is very basic.

- `#BEGIN_SNIPPET name` where you indicate that a new snippet starts here with
  the given name

- `#END_SNIPPET name` to indicate that the snippet stops here, the `name` is
  optional

- `_1_` somewhere inside the snippet which is where the cursor position will be
  after the snippet expansion.

- `___` optionally, you can add more place to jump to using `n` and `N` (it
  utilises the search register `/`, so it's basically just searching forwards
  and backwards)

Complete example:

```
#BEGIN_SNIPPET name_of_snippet
func(_1_, ___, ___, ___);
#END_SNIPPET name_of_snippet
```

## Install

First, you have to build it:

```
make
```

Then you copy the `cnippet` executable and the `snippets.txt` file which
contains the snippets to your Helix config path.

Or you can do that with:

```
make install
```

After that you have to add this keybinding to your Helix config:

```toml
[keys.insert]
C-e = [
  "move_prev_word_start",  "trim_selections",  ":pipe ~/.config/helix/cnippet",
  ":insert-output echo _1_", "search_selection", "delete_selection", "search_next", "delete_selection",
  ":insert-output echo ___",  "search_selection",  "delete_selection", "insert_mode"
]
```

(You can also use `make debug` for development)

## Usage

Now, while in insert mode, you can type a snippet like `a` and then press `C-e` 
for it to expand. After that you can press `n` and `N` to jump to the next 
position, if there are any `___`.

## Alternatives 

- <https://quantonganh.com/2023/07/31/create-snippets-in-helix.md>
- <https://www.reddit.com/r/HelixEditor/comments/zk1ao5/how_to_create_language_snippets_in_helix/>

