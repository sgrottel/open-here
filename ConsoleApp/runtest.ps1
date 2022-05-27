param(
    [Parameter(Mandatory)][string]$binPath
    [Parameter(Mandatory)][string]$repoPath
    )

Write-Host
Write-Host "Init:"
$lines1 = & "$binPath\OpenHereCon.exe"
$lines1

Write-Host
Write-Host "Starting Explorer #1"
Write-Host "/E, `"$binPath`""
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe "/E," "$binPath"
for ($i = 0; $i -lt 50; ++$i) {
    $nexps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
    if ($nexps.length -gt $exps.length) {
        break;
    }
    Sleep -Milliseconds 100
}
Sleep -Milliseconds 500

Write-Host
Write-Host "Detecting:"
$lines2 = & "$binPath\OpenHereCon.exe"
$lines2

Write-Host
Write-Host "Starting Explorer #2"
Write-Host "/SELECT, `"$repoPath\Version.h`""
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe "/SELECT," "$repoPath\Version.h"
for ($i = 0; $i -lt 50; ++$i) {
    $nexps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
    if ($nexps.length -gt $exps.length) {
        break;
    }
    Sleep -Milliseconds 100
}
Sleep -Milliseconds 500

Write-Host
Write-Host "Detecting:"
$lines3 = & "$binPath\OpenHereCon.exe"
$lines3
