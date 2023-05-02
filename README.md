## CaoVM

**This is a hobby project, and probably won't be useful to you, feel free to check it out either way!**

My personal implementation of the [Java Virtual Machine](https://docs.oracle.com/javase/specs/jvms/se17/html/index.html), made in C++.

This project relies heavily on the [Lagom](https://github.com/SerenityOS/serenity/tree/master/Meta/Lagom) collection of libraries from [SerenityOS](https://github.com/SerenityOS/serenity).

**But why do you not just implement this in SerenityOS?**

Well, someone started that before, but as the [creator Andreas said](https://github.com/SerenityOS/serenity/pull/11500#issuecomment-1003134109), the JVM isn't really something that makes sense for SerenityOS, and I agree with that sentiment. The JVM is huge, and it would be pretty useless in SerenityOS apart from a few people who may be interested (like me).

## Building

> **Note**:
> It is recommended to set the `SERENITY_SOURCE_DIR` environment variable to the path of your locally cloned SerenityOS folder, if you have one.

- Clone the repository, and run the `./Scripts/setup.sh` script.

- Then, you can build and execute the program with `./Scripts/build-and-run.sh`.

## Recommended Visual Studio Code settings

`.vscode/settings.json`

```jsonc
{
  // Excluding the generated directories keeps your file view clean and speeds up search.
  "files.exclude": {
    "**/.git": true,
    "Toolchain/Local/**": true,
    "Toolchain/Tarballs/**": true,
    "Toolchain/Build/**": true,
    "**/.devcontainer": true
  },
  "search.exclude": {
    "**/.git": true,
    "Toolchain/Local/**": true,
    "Toolchain/Tarballs/**": true,
    "Toolchain/Build/**": true,
    "Build/**": true,
    "build/**": true
  },
  // Tab settings
  "editor.tabSize": 4,
  "editor.useTabStops": false,
  // format trailing new lines
  "files.trimFinalNewlines": true,
  "files.insertFinalNewline": true,
  // git commit message length
  "git.inputValidationLength": 72,
  "git.inputValidationSubjectLength": 72,
  "git.useEditorAsCommitInput": true,
  // Tell clangd to ask the cross-compilers for their builtin include paths
  "clangd.arguments": [
    "--header-insertion=never" // See https://github.com/clangd/clangd/issues/1247
  ]
}
```
