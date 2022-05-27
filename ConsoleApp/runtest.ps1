param(
    [Parameter(Mandatory)][string]$binPath,
    [Parameter(Mandatory)][string]$repoPath
    )

Write-Host
Write-Host "Init:"
$lines1 = & "$binPath\OpenHereCon.exe"
$lines1

Write-Host
Write-Host "Starting Explorer #1"
$dir = Resolve-Path $binPath
Write-Host "/E,`"$dir`""
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe "/E,`"$dir`""
for ($i = 0; $i -lt 50; ++$i) {
    $nexps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
    if ($nexps.length -gt $exps.length) {
        break;
    }
    Sleep -Milliseconds 100
}
Sleep -Milliseconds 2000

Write-Host
Write-Host "Detecting:"
$lines2 = & "$binPath\OpenHereCon.exe"
$lines2

Write-Host
Write-Host "Starting Explorer #2"
$dir = Resolve-Path $repoPath
Write-Host "/SELECT,`"$dir\Version.h`""
$exps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
explorer.exe "/SELECT,`"$dir\Version.h`""
for ($i = 0; $i -lt 50; ++$i) {
    $nexps = Get-Process -Name '*explorer*' | Where-Object { $_.MainWindowHandle -gt 0 }
    if ($nexps.length -gt $exps.length) {
        break;
    }
    Sleep -Milliseconds 100
}
Sleep -Milliseconds 10000

Write-Host
Write-Host "Detecting:"
$lines3 = & "$binPath\OpenHereCon.exe"
$lines3
