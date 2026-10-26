# How to contribute

As an open-source project, we welcome and encourage the community to submit
patches directly to the project. As a contributor you should be familiar with
common developer tools such as Git and CMake, and platforms such as GitHub.

Then following points should be respected to facilitate the review process.

## Licencing

Code is contributed to OpenAMP under a number of licenses, but all code must be
compatible with version the
[BSD License](https://github.com/OpenAMP/libmetal/blob/master/LICENSE.md), which
is the license covering the OpenAMP distribution as a whole. In practice, use
the following tag instead of the full license text in the individual files:

```
SPDX-License-Identifier:    BSD-3-Clause
```
## Signed-off-by

Commit messages must contain Signed-off-by: line and your email must match the
change authorship information. Make sure your .gitconfig is set up correctly:

```shell
git config --global user.name "first-name Last-Namer"
git config --global user.email "yourmail@company.com"
```

## Assisted-by
When AI tools are used to contribute to the OpenAMP project, proper attribution helps
track the evolving role of AI in the development process. Contributions should include
an Assisted-by tag in the following format:

Assisted-by: AGENT_NAME:MODEL_VERSION [TOOL1] [TOOL2]

Where:

AGENT_NAME is the name of the AI tool or framework
MODEL_VERSION is the specific model version used
[TOOL1] [TOOL2] are optional specialized analysis tools used, for example, coccinelle,
sparse, and smatch
Basic development tools (git, gcc, make, editors) should not be listed.

Example: Assisted-by: Claude:claude-3-opus coccinelle sparse

## gitlint

Before you submit a pull request to the project, verify your commit messages
meet the requirements. The check can be performed locally using the the gitlint
command.

Run gitlint locally in your tree and branch where your patches have been
committed:

```shell
gitlint
```

Note, gitlint only checks HEAD (the most recent commit), so you should run it
after each commit, or use the --commits option to specify a commit range
covering all the development patches to be submitted.

## Code style

In general, follow the Linux kernel coding style, with the following exceptions:

* Use /**  */ for doxygen comments that need to appear in the documentation.

The Linux kernel GPL-licensed tool checkpatch is used to check coding style
conformity. Checkpatch is available in the scripts directory.

To check your \<n\> commits in your git branch:

```shell
./scripts/checkpatch.pl --strict  -g HEAD-<n>
```
## Send a pull request

We use standard GitHub mechanism for pull request. Please refer to GitHub
documentation for help.
