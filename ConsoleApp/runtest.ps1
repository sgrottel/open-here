param([Parameter(Mandatory)][string]$binPath)

$lines = & "$binPath\OpenHereCon.exe"
$lines
