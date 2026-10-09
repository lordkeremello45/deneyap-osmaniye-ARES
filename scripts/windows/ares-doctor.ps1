# ARES Windows preflight diagnostics
# Read-only: does not install software, change firewall rules, start services, or print secrets.
[CmdletBinding()]
param(
    [ValidateSet('Developer', 'Runtime')]
    [string]$Mode = 'Developer',
    [switch]$RequireMqtt,
    [string]$ModelPath = (Join-Path $PSScriptRoot '..\..\models\google_gemma-4-E2B-it-Q5_K_M.gguf')
)

$ErrorActionPreference = 'Stop'
$script:Failures = 0
$script:Warnings = 0
$script:Results = [System.Collections.Generic.List[object]]::new()

function Add-Check {
    param([string]$Name, [ValidateSet('PASS','WARN','FAIL','INFO')][string]$Status, [string]$Detail)
    $script:Results.Add([pscustomobject]@{ check = $Name; status = $Status; detail = $Detail })
    $line = '[{0}] {1}: {2}' -f $Status, $Name, $Detail
    switch ($Status) {
        'FAIL' { $script:Failures++; Write-Error $line -ErrorAction Continue }
        'WARN' { $script:Warnings++; Write-Warning $line }
        default { Write-Host $line }
    }
}

function Find-CommandPath {
    param([string]$Name)
    $command = Get-Command $Name -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($null -eq $command) { return $null }
    return $command.Source
}

function Get-CommandVersion {
    param([string]$Name, [string[]]$Arguments = @('--version'))
    $path = Find-CommandPath $Name
    if (-not $path) { return $null }
    try {
        $output = & $path @Arguments 2>&1 | Select-Object -First 1
        return [string]$output
    } catch { return 'installed; version query failed' }
}

Add-Check 'Operating system' $(if ($env:OS -eq 'Windows_NT') { 'PASS' } else { 'FAIL' }) $(if ($env:OS -eq 'Windows_NT') { [System.Runtime.InteropServices.RuntimeInformation]::OSDescription } else { 'This diagnostic must run on Windows.' })
Add-Check 'PowerShell' $(if ($PSVersionTable.PSVersion.Major -ge 5) { 'PASS' } else { 'FAIL' }) $PSVersionTable.PSVersion.ToString()
Add-Check 'Process architecture' 'INFO' ([System.Runtime.InteropServices.RuntimeInformation]::ProcessArchitecture.ToString())

$requiredCommands = if ($Mode -eq 'Developer') { @('git', 'cmake', 'go') } else { @() }
$optionalCommands = @('flutter', 'python', 'pio', 'mosquitto', 'llama-server')
foreach ($name in $requiredCommands) {
    $version = Get-CommandVersion $name
    if ($version) { Add-Check "Tool: $name" 'PASS' $version }
    else { Add-Check "Tool: $name" 'FAIL' 'Not found on PATH; install it and reopen the terminal.' }
}
foreach ($name in $optionalCommands) {
    $version = Get-CommandVersion $name
    if ($version) { Add-Check "Tool: $name" 'PASS' $version }
    else { Add-Check "Tool: $name" 'INFO' 'Not found; optional for this diagnostic mode or must be installed before its subsystem is used.' }
}

try {
    $os = Get-CimInstance Win32_OperatingSystem
    $freeGB = [math]::Round(([double]$os.FreePhysicalMemory / 1MB), 2)
    $totalGB = [math]::Round(([double]$os.TotalVisibleMemorySize / 1MB), 2)
    Add-Check 'Physical memory' $(if ($totalGB -ge 8) { 'PASS' } else { 'WARN' }) ("{0} GB total; {1} GB currently free. Gemma inference needs a separate RAM/performance benchmark." -f $totalGB, $freeGB)
} catch { Add-Check 'Physical memory' 'WARN' 'Could not query memory through CIM.' }

try {
    $drive = Get-PSDrive -Name $env:SystemDrive.TrimEnd(':') -ErrorAction Stop
    $freeDiskGB = [math]::Round($drive.Free / 1GB, 2)
    Add-Check 'System drive free space' $(if ($freeDiskGB -ge 5) { 'PASS' } else { 'WARN' }) ("{0} GB free. Model downloads/build artifacts may require additional space." -f $freeDiskGB)
} catch { Add-Check 'System drive free space' 'WARN' 'Could not determine free disk space.' }

if (Test-Path -LiteralPath $ModelPath -PathType Leaf) {
    $modelBytes = (Get-Item -LiteralPath $ModelPath).Length
    Add-Check 'Gemma model file' 'PASS' ("Present ({0} GB); this does not prove that inference works." -f [math]::Round($modelBytes / 1GB, 2))
} else {
    Add-Check 'Gemma model file' 'INFO' 'Not present at the configured path. Download separately only if local inference is being tested.'
}

$mosquittoPath = Find-CommandPath 'mosquitto'
$mosquittoService = Get-Service -Name 'mosquitto' -ErrorAction SilentlyContinue
if ($mosquittoPath) {
    Add-Check 'Mosquitto executable' 'PASS' $mosquittoPath
} elseif ($RequireMqtt -or $Mode -eq 'Runtime') {
    Add-Check 'Mosquitto executable' 'FAIL' 'Mosquitto is required in this mode but was not found on PATH.'
} else {
    Add-Check 'Mosquitto executable' 'INFO' 'Not installed; MQTT is not required for the current development preflight.'
}
if ($null -ne $mosquittoService) {
    Add-Check 'Mosquitto service' $(if ($mosquittoService.Status -eq 'Running') { 'PASS' } else { 'WARN' }) ("Service state: {0}. This check does not validate TLS, authentication, ACLs, or listener exposure." -f $mosquittoService.Status)
} elseif ($RequireMqtt -or $Mode -eq 'Runtime') {
    Add-Check 'Mosquitto service' 'FAIL' 'Windows service not found; broker lifecycle is not ready.'
} else {
    Add-Check 'Mosquitto service' 'INFO' 'Service not found; expected until MQTT integration is installed.'
}

$summary = [pscustomobject]@{
    schemaVersion = 1
    mode = $Mode
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    failureCount = $script:Failures
    warningCount = $script:Warnings
    checks = @($script:Results)
}
$jsonPath = Join-Path (Get-Location) 'ares-windows-preflight.json'
try {
    $summary | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $jsonPath -Encoding utf8
    Write-Host "Diagnostic report written to: $jsonPath"
} catch { Write-Warning 'Could not write JSON report in the current directory.' }

Write-Host ("ARES preflight completed: {0} failure(s), {1} warning(s)." -f $script:Failures, $script:Warnings)
if ($script:Failures -gt 0) { exit 1 }
exit 0
