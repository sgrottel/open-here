param(
    [Parameter(Mandatory)][string]$binPath,
    [Parameter(Mandatory)][string]$repoPath
    )

$shell = New-Object -ComObject Shell.Application

function SelectFiles($filesToSelect)
{
    foreach ($fileToSelect in $filesToSelect)
    {
        foreach ($window in $shell.Windows())
        {
            foreach ($folderItem in $window.Document.Folder.Items())
            {
                if ($folderItem.Path -eq $fileToSelect)
                {
                    $window.Document.SelectItem($folderItem, 1 + 8)
                }
            }
        }
    }
}

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
Sleep -Milliseconds 500

Write-Host
Write-Host "Detecting:"
$lines2 = & "$binPath\OpenHereCon.exe"
$lines2

$m = $lines2 -match "^i\s+\d+\s+exploorer\.exe\s*$"
if (($m -is [string]) -or (($m -is [array]) -and ($m.length -gt 0))) {
    Write-Host "i found"
} else {
    Write-Error "No explorer instance found!"
}

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
Sleep -Milliseconds 500
SelectFiles(@("$dir\Version.h"))
Sleep -Milliseconds 500

Write-Host
Write-Host "Detecting:"
$lines3 = & "$binPath\OpenHereCon.exe"
$lines3
