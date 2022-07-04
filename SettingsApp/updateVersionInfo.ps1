# Open Here
# Copyright 2022 SGrottel (https://www.sgrottel.de)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http ://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissionsand
# limitations under the License.
#
cd $PSScriptRoot

$vf = get-content "..\Version.h" -Raw

$vf -match '\n#define\s+OPEN_HERE_VER_MAJOR\s+(\d+)\s*\n' | Out-Null
$major = $Matches[1]
$vf -match '\n#define\s+OPEN_HERE_VER_MINOR\s+(\d+)\s*\n' | Out-Null
$minor = $Matches[1]
$vf -match '\n#define\s+OPEN_HERE_VER_PATCH\s+(\d+)\s*\n' | Out-Null
$patch = $Matches[1]
$vf -match '\n#define\s+OPEN_HERE_VER_BUILD\s+(\d+)\s*\n' | Out-Null
$build = $Matches[1]
$vf -match '\n#define\s+OPEN_HERE_VER_YEARSTR\s+"([^"]+)"\s*\n' | Out-Null
$year = $Matches[1]

$prodVer = "$major.$minor.$patch.0"
$fileVer = "$major.$minor.$patch.$build"
$copyrightStr = "Copyright $year, SGrottel"

# $prodVer
# $fileVer
# $copyrightStr

$csproj = [System.XML.XMLDocument]::new()
$csproj.PreserveWhitespace = $true
$csprojPath = Resolve-Path ".\SettingsApp.csproj"
$csproj.Load($csprojPath)

$updated = $false

if ($csproj.Project.PropertyGroup.Version -ne $prodVer)
{
	Write-Host "Update product version: '$($csproj.Project.PropertyGroup.Version)' -> '$prodVer'"
	$csproj.Project.PropertyGroup.Version = $prodVer
	$updated = $true
}
if ($csproj.Project.PropertyGroup.AssemblyVersion -ne $fileVer)
{
	Write-Host "Update assembly version: '$($csproj.Project.PropertyGroup.AssemblyVersion)' -> '$fileVer'"
	$csproj.Project.PropertyGroup.AssemblyVersion = $fileVer
	$updated = $true
}
if ($csproj.Project.PropertyGroup.FileVersion -ne $fileVer)
{
	Write-Host "Update file version: '$($csproj.Project.PropertyGroup.FileVersion)' -> '$fileVer'"
	$csproj.Project.PropertyGroup.FileVersion = $fileVer
	$updated = $true
}
if ($csproj.Project.PropertyGroup.Copyright -ne $copyrightStr)
{
	Write-Host "Update copyright string: '$($csproj.Project.PropertyGroup.Copyright)' -> '$copyrightStr'"
	$csproj.Project.PropertyGroup.Copyright = $copyrightStr
	$updated = $true
}

if ($updated)
{
	$writerSettings = [System.Xml.XmlWriterSettings]::new()
	$writerSettings.OmitXmlDeclaration = $true;
	$writerSettings.Encoding = [Text.Encoding]::UTF8
	$writer = [System.Xml.XmlWriter]::Create($csprojPath, $writerSettings)
	
	$csproj.Save($writer)
	
	$writer.close()
	$writer.dispose()
	$writer = $null
} else {
	Write-Host "No update necessary"
}
