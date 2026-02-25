param(
    [Parameter(Mandatory = $true)]
    [string]$UiFile
)

if (-not $UiFile -or -not (Test-Path $UiFile)) {
    Write-Error "No active file selected."
    exit 1
}

if ([IO.Path]::GetExtension($UiFile) -ne ".ui") {
    Write-Error "Active file is not a .ui file."
    exit 1
}

$qt = Get-Command qtcreator.exe -ErrorAction SilentlyContinue
if ($qt) {
    Start-Process -FilePath $qt.Source -ArgumentList $UiFile
    exit 0
}

$common = @(
    "F:\Qt\Tools\QtCreator\bin\qtcreator.exe",
    "C:\Qt\Tools\QtCreator\bin\qtcreator.exe",
    "C:\Qt\Qt Creator\bin\qtcreator.exe"
)

$found = $common | Where-Object { Test-Path $_ } | Select-Object -First 1
if ($found) {
    Start-Process -FilePath $found -ArgumentList $UiFile
    exit 0
}

Write-Error "Qt Creator not found in PATH or common install paths."
exit 1
