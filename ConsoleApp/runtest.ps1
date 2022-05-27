param([Parameter(Mandatory)][string]$binPath)

Write-Host
Write-Host "Init:"
$lines1 = & "$binPath\OpenHereCon.exe"
Write-Host $lines1

Write-Host
Write-Host "Starting Explorer #1"
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe /E, "$binPath"
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
Write-Host $lines2

Write-Host
Write-Host "Starting Explorer #2"
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe /SELECT, "$binPath\..\..\..\Version.h"
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
Write-Host $lines3
