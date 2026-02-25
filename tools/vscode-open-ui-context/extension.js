const vscode = require('vscode');
const cp = require('child_process');
const path = require('path');

function getUiPath(resourceUri) {
  if (resourceUri && resourceUri.fsPath) {
    return resourceUri.fsPath;
  }
  const active = vscode.window.activeTextEditor;
  if (active && active.document && active.document.uri) {
    return active.document.uri.fsPath;
  }
  return '';
}

function activate(context) {
  const disposable = vscode.commands.registerCommand('nolimit.openUiInQtCreator', (resourceUri) => {
    const workspaceFolder = vscode.workspace.workspaceFolders && vscode.workspace.workspaceFolders[0];
    if (!workspaceFolder) {
      vscode.window.showErrorMessage('No workspace folder is open.');
      return;
    }

    const uiPath = getUiPath(resourceUri);
    if (!uiPath || path.extname(uiPath).toLowerCase() !== '.ui') {
      vscode.window.showErrorMessage('Select a .ui file first.');
      return;
    }

    const scriptPath = path.join(workspaceFolder.uri.fsPath, '.vscode', 'open-ui-in-qtcreator.ps1');
    const args = [
      '-NoProfile',
      '-ExecutionPolicy',
      'Bypass',
      '-File',
      scriptPath,
      '-UiFile',
      uiPath
    ];

    const proc = cp.spawn('powershell', args, { windowsHide: true });

    let stderr = '';
    proc.stderr.on('data', (data) => {
      stderr += data.toString();
    });

    proc.on('error', (err) => {
      vscode.window.showErrorMessage(`Failed to start PowerShell: ${err.message}`);
    });

    proc.on('close', (code) => {
      if (code !== 0) {
        const details = stderr.trim() || 'Unknown error.';
        vscode.window.showErrorMessage(`Open in Qt Creator failed: ${details}`);
      }
    });
  });

  context.subscriptions.push(disposable);
}

function deactivate() {}

module.exports = {
  activate,
  deactivate
};
