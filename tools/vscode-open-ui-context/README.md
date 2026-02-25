# NoLimit Open UI in Qt Creator (local extension)

Adds **Open in Qt Creator** to the right-click menu for `.ui` files in Explorer and editor tabs.

## Use it now (no install)

1. Open this folder in VS Code: `tools/vscode-open-ui-context`
2. Press `F5` to launch an Extension Development Host
3. In the new window, open your project folder and right-click any `.ui` file
4. Click **Open in Qt Creator**

## Optional: package and install

If you want it available in your normal VS Code instance without dev host:

1. Install `vsce` (`npm i -g @vscode/vsce`)
2. From this folder run `vsce package`
3. Install the generated `.vsix` in VS Code

The command uses the existing script at `.vscode/open-ui-in-qtcreator.ps1`.
