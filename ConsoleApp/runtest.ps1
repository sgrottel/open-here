param([Parameter(Mandatory)][string]$binPath)

$lines1 = & "$binPath\OpenHereCon.exe"
$lines1

explorer.exe /E, "$binPath"

$lines2 = & "$binPath\OpenHereCon.exe"
$lines2

explorer.exe /SELECT, "$binPath\..\..\..\Version.h"

$lines3 = & "$binPath\OpenHereCon.exe"
$lines3

