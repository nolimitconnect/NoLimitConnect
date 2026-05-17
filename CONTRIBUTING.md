# Contributing to NoLimitConnect

Thank you for your interest in helping NoLimitConnect.

This repository currently uses a lightweight open source workflow suitable for a single maintainer and external contributors.

## Contribution Model

- External contributors: use fork + pull request.
- Maintainer: reviews and merges pull requests.
- Default branch: main.

No direct write access is required to contribute.

## Quick Start (Fork + PR)

1. Fork the repository on GitHub.
2. Clone your fork locally.
3. Create a branch for your change.
4. Commit and push your branch to your fork.
5. Open a pull request to the upstream main branch.

Example:

```bash
git clone https://github.com/<your-user>/NoLimitConnect.git
cd NoLimitConnect
git checkout -b docs/plugin-system-overview
# edit files
git add .
git commit -m "docs: add plugin system overview"
git push -u origin docs/plugin-system-overview
```

Then open a PR from your fork branch to:

- base repository: nolimitconnect/NoLimitConnect
- base branch: main

## What To Work On First

Good first contributions:

- Documentation fixes and clarifications under docs/
- Small bug fixes with clear reproduction steps
- Build or packaging script improvements
- Test and CI reliability improvements

If unsure, open an issue first and describe your proposed change.

## Local Docs Preview

This project website/docs use MkDocs.

```bash
pip install mkdocs-material
mkdocs serve
```

Default local URL:

- http://127.0.0.1:8000/

## Pull Request Guidelines

- Keep PRs focused and reasonably small.
- Write clear commit messages.
- Include context: what changed, why, and how it was tested.
- For docs/UI changes, include screenshots when helpful.
- Link related issue numbers when applicable.

## Coding and Review Expectations

- Preserve existing style and architecture unless a change is intentionally refactoring.
- Avoid unrelated changes in the same PR.
- Prefer follow-up PRs over one very large PR.

## Security

Do not report security issues in public issues.

Use:

- SECURITY.md

## Need Help?

- Open a GitHub issue with the question.
- For project leadership/community roles, see project contact details in README.md.
