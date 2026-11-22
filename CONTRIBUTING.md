# Contributing to POV-Ray

Thank you for wanting to improve POV-Ray. This page says what we ask of a contribution and how we decide what goes in.

## Before you start

- For a bug, please mention it in the [forums](https://news.povray.org/) or check the [known issues](https://github.com/POV-Ray/povray/issues) first; see the Bug Reports section of the [README](README.md).
- For a larger change, open an issue describing what you intend before writing the code. It is much easier to agree on a design than to rework a finished pull request.

## Pull requests

- Base your work on the `master` branch. Fixes that also belong on a release branch are carried there by the maintainers, keeping you as the author.
- Keep each pull request to one topic. A bug fix and an unrelated clean-up are two pull requests.
- Follow the coding conventions in [source-doc/styleguide.md](source-doc/styleguide.md).
- Say what you tested, and on which platform and compiler. POV-Ray builds on Windows, Linux, macOS and others, and a change to shared code must keep building and rendering correctly on all of them.
- A change that alters rendered output needs to say so, and why.

## Licence

POV-Ray's source is licensed under the AGPL3 (see [LICENSE](LICENSE)). By submitting a contribution you confirm that you have the right to submit it and that it may be distributed under that licence. If the work was done for an employer or a client, please make sure they agree before you submit it.

## Use of AI tools

Contributions are judged on their merits, whatever tools were used to produce them. Code written with the help of a large language model (LLM) or a similar tool is welcome on the same terms as any other code, and those terms are:

1. **A person is responsible for it.** Every contribution is submitted by a person who has read and understood all of it, has tested it, and will answer questions about it in review. "The tool wrote it that way" is not an answer.
2. **The person vouches for its origin.** The Licence section above applies in full. You are confirming that the work can be contributed under the AGPL3, however it was produced.
3. **Say that a tool was used.** A line in the commit message or the pull request description is enough, for example a `Co-authored-by` trailer or a sentence naming the tool. We do this ourselves. Disclosure does not count against a contribution; finding out later that it was left out does.
4. **The same review applies.** A contribution has to be valid, useful and well designed, or fix a genuine bug. We do not accept less from a tool-assisted contribution, and we do not ask more of it.
5. **Unreviewed output is not a contribution.** Pull requests, issues or comments that are the unexamined output of a tool, or whose author cannot explain them, will be closed. This includes automatically generated bug reports and security reports that nobody has verified.

Some people would prefer a project with no AI-assisted code at all. We understand that view and have chosen differently: what we care about is the quality of the work and that a person stands behind it.

## Conduct

Be civil. Disagreement about code is normal and welcome; disrespect towards the people writing or reviewing it is not.
