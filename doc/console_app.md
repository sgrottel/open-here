# Open Here Command Line Application
The command line application `OpenHereCon.exe` prints the information about detected file explorer instances to the standard output channel.
The text is printed in a why which is both readable and easy to parse.

## Output
The output is sorted to show the information of the top-most file explorer application instance first.

Each line of output starts with a single character, followed by a space character:

`i` -- Instance -- This line starts the information block of one file explorer application instance.

The line prefix is followed by one number.
This is the file explorer application's main [window handle](https://docs.microsoft.com/en-us/windows/win32/winmsg/about-windows).

Following the number, separated by one space, is a string representing the type of file explorer.
For example: `explorer.exe`

`w` -- Window -- some information about the file explorers main window.

The first two characters following the prefix are:
* `T` or `-` if the window is marked as the top window of the depth stack
* `F` or `-` if the window is marked as a foreground window.

The depth-position number is following those numbers, where a smaller number indicates the window being further to the top of the depth stack.

`p` -- Path -- the path that is opened in the file explorer.

`s` -- Selected -- the file system items selected in the file explorer.

Those can be files or paths or abstract objects.

`e` -- End -- is printed at the very end of the application output.

This indicates a clean application exit.


## Exit Code
The application returns `0` (Zero) on success.

Any other number indicates an error.
Check the application's output, both on the standard output stream and the standard error stream for more information.


## Example

### Simple Run
```ps
.\openherecon.exe
```

```
i 2821148 explorer.exe
w -- 53
p \\###network-share###\data\downloads
s \\###network-share###\data\downloads\###file-1.###
s \\###network-share###\data\downloads\###file-2.###
s \\###network-share###\data\downloads\###file-3.###
i 787026 explorer.exe
w -- 68
p C:\dev\open-here\FileExplorerDetector
s C:\dev\open-here\FileExplorerDetector\CoInitialize.h
e
```
Two Microsoft Windows File Explorer instances are running.

One is opened on a network share and three files are selected.

The second is opened in a checkout directory of this repository, with one file selected.

### Usage in Script #1
The following powershell script opens the selected files in Notepad++:
```ps
cd $PSScriptRoot
$lines = & .\OpenHereCon.exe
if ($lines -is [string]) {
	$lines = @($lines)
}
if (-not ($lines -is [array])) {
    Write-Error "Output of OpenHereCon.exe is unexpectedly not an array."
    exit
}

# Search for first instance
while ($lines.length -gt 0) {
    $line, $lines = $lines
    if ($line.StartsWith("i ")) { break }
}

# Collect files
$files = @()
while ($lines.length -gt 0) {
    $line, $lines = $lines
    if ($line.StartsWith("i ")) { break }
    if ($line.StartsWith("e ")) { break }
    if ($line.StartsWith("s ")) {
        $file = $line.Substring(2)
        if ((Get-Item $file) -is [System.IO.FileInfo]) {
            $files += "`"$file`""
        }
    }
}

# Open Notepad++
notepad++.exe $files
```

### Usage in Script #2
The following powershell script opens the windows terminal in the path of the first opened folder:
```ps
cd $PSScriptRoot
$lines = & .\OpenHereCon.exe
if ($lines -is [string]) {
	$lines = @($lines)
}
if (-not ($lines -is [array])) {
    Write-Error "Output of OpenHereCon.exe is unexpectedly not an array."
    exit
}

# Search for first instance
while ($lines.length -gt 0) {
    $line, $lines = $lines
    if ($line.StartsWith("i ")) { break }
}

# Collect first path
$args = @()
while ($lines.length -gt 0) {
    $line, $lines = $lines
    if ($line.StartsWith("i ")) { break }
    if ($line.StartsWith("e ")) { break }
    if ($line.StartsWith("p ")) {
        $args += "-wd"
        $args += ("`"" + $line.Substring(2) + "`"")
        break
    }
}

# open powershell in new windows terminal window
wt pwsh $args -NoExit -Interactive
```
