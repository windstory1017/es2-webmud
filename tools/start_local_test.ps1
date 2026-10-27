param(
  [int]$MudPort = 4101,
  [int]$WebPort = 8181
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$sourceRoot = Join-Path $root "source\upstream"
$sourceMudlib = Join-Path $sourceRoot "mudlib"
$neo = Join-Path $sourceRoot "neolith\build-vs2022\src\Release\neolith.exe"
$server = Join-Path $root "server"
$localRoot = Join-Path $root ".local-test"
$runtime = Join-Path $localRoot "runtime"
$runtimeMudlib = Join-Path $runtime "mudlib"
$runtimeConf = Join-Path $runtime "neolith.localtest.conf"
$pidFile = Join-Path $localRoot "localtest-pids.json"
$seedRoot = Join-Path $root "tools\local_test_seed"

function Step([string]$text) { Write-Host "[ES2 Local Test] $text" -ForegroundColor Cyan }
function Test-Port([int]$Port) {
  try {
    $c = New-Object System.Net.Sockets.TcpClient
    $ar = $c.BeginConnect("127.0.0.1",$Port,$null,$null)
    $ok = $ar.AsyncWaitHandle.WaitOne(250)
    if($ok){$c.EndConnect($ar)}
    $c.Close(); return $ok
  } catch { return $false }
}
function Wait-Port([int]$Port,[int]$Seconds) {
  $deadline=(Get-Date).AddSeconds($Seconds)
  do { if(Test-Port $Port){return $true}; Start-Sleep -Milliseconds 350 } while((Get-Date)-lt $deadline)
  return $false
}

Step "Checking prerequisites..."
if(!(Test-Path $neo)){ throw "Neolith executable not found: $neo" }
if(!(Test-Path $sourceMudlib)){ throw "mudlib not found: $sourceMudlib" }
if(!(Test-Path (Join-Path $server "package.json"))){ throw "server/package.json not found" }
if(!(Get-Command node.exe -ErrorAction SilentlyContinue)){ throw "Node.js is not installed or not in PATH." }
if(!(Get-Command npm.cmd -ErrorAction SilentlyContinue)){ throw "npm is not installed or not in PATH." }
if(!(Get-Command robocopy.exe -ErrorAction SilentlyContinue)){ throw "robocopy.exe not found." }
if(Test-Port $MudPort){ throw "Local test MUD port $MudPort is already in use. Run the local-test stop script first." }
if(Test-Port $WebPort){ throw "Local test Web port $WebPort is already in use. Run the local-test stop script first." }

# IMPORTANT: Never edit/copy-convert LPC source text. Robocopy preserves bytes exactly.
# log/ and bin/ are local-test-private. data/user, data/login, and data/mail are private, while required system data files are synced.
New-Item -ItemType Directory -Force -Path $runtimeMudlib | Out-Null
Step "Syncing game code byte-for-byte into isolated runtime..."
$exclude = @(
  (Join-Path $sourceMudlib "log"),
  (Join-Path $sourceMudlib "bin"),
  (Join-Path $sourceMudlib "data\user"),
  (Join-Path $sourceMudlib "data\login"),
  (Join-Path $sourceMudlib "data\mail")
)
$roboArgs = @(
  $sourceMudlib, $runtimeMudlib,
  "/MIR","/COPY:DAT","/DCOPY:DAT","/IS","/IT","/R:1","/W:1",
  "/NFL","/NDL","/NJH","/NJS","/NP","/XJ",
  "/XD"
) + $exclude
& robocopy.exe @roboArgs | Out-Null
$roboCode=$LASTEXITCODE
if($roboCode -ge 8){ throw "Robocopy failed with code $roboCode" }

# Private local-test writable state. Never copied back to source/upstream/mudlib.
# IMPORTANT: system data such as data/chinese.o, data/daemon.o, data/emoted.o and data/daemon/*
# must be present because the MUD login/runtime depends on them. Only player/account/mail state is isolated.
$runtimeData = Join-Path $runtimeMudlib "data"
$runtimeLog = Join-Path $runtimeMudlib "log"
$runtimeBin = Join-Path $runtimeMudlib "bin"
foreach($d in @($runtimeData,$runtimeLog,$runtimeBin,(Join-Path $runtimeData "user"),(Join-Path $runtimeData "login"),(Join-Path $runtimeData "mail"))){
  New-Item -ItemType Directory -Force -Path $d | Out-Null
}

# Seed only the public local-test admin. Formal player/account files are never imported.
$userSeed = Join-Path $seedRoot "user\t\testadmin.o"
$loginSeed = Join-Path $seedRoot "login\t\testadmin.o"
if(!(Test-Path $userSeed)){ throw "Missing local-test user seed: $userSeed" }
if(!(Test-Path $loginSeed)){ throw "Missing local-test login seed: $loginSeed" }
New-Item -ItemType Directory -Force -Path (Join-Path $runtimeData "user\t") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $runtimeData "login\t") | Out-Null
if(!(Test-Path (Join-Path $runtimeData "user\t\testadmin.o"))){ Copy-Item -Force $userSeed (Join-Path $runtimeData "user\t\testadmin.o") }
if(!(Test-Path (Join-Path $runtimeData "login\t\testadmin.o"))){ Copy-Item -Force $loginSeed (Join-Path $runtimeData "login\t\testadmin.o") }

# This is the ONLY runtime LPC-side adjustment: an ASCII-only wizard-list append inside the isolated copy.
# No formal source file is modified.
$wizlist = Join-Path $runtimeMudlib "adm\etc\wizlist"
if(Test-Path $wizlist){
  $wiz = [IO.File]::ReadAllText($wizlist,[Text.Encoding]::UTF8)
  if($wiz -notmatch '(?m)^testadmin\s+\(arch\)\s*$'){
    [IO.File]::AppendAllText($wizlist,"`ntestadmin (arch)`n",[Text.Encoding]::ASCII)
  }
}

# Generate a local-only Neolith config. No LPC source file is rewritten.
$conf = @"
MudlibDir mudlib
MudName Eastern Stories II Local Test
Port $($MudPort):telnet
LogDir log
DebugLogFile debug.log
LogWithDate Yes
CrashDropCore No
IncludeDir /include
GlobalInclude <globals.h>
SaveBinaryDir /bin
MasterFile /adm/obj/master.c
SimulEfunFile /adm/obj/simul_efun.c
DefaultErrorMsg Oops, you feel something weird just happend.
DefaultFailMsg What?
CleanupDuration 600
ResetDuration 900
MaxInheritDepth 30
MaxEvaluationCost 1000000
MaxArraySize 15000
MaxMappingSize 15000
MaxBufferSize 400000
MaxStringLength 200000
MaxBitFieldBits 1200
MaxByteTransfer 10000
MaxReadFileSize 200000
SharedStringHashSize 20011
ObjectHashSize 10007
StackSize 1000
MaxLocalVariables 25
MaxCallDepth 50
ArgumentsInTrace Yes
LocalVariablesInTrace Yes
"@
[IO.File]::WriteAllText($runtimeConf,$conf,(New-Object Text.ASCIIEncoding))

# Verify required non-player system data exists in the isolated runtime.
$requiredSystemData = @(
  "data\chinese.o",
  "data\daemon.o",
  "data\emoted.o"
)
foreach($rel in $requiredSystemData){
  $src = Join-Path $sourceMudlib $rel
  $dst = Join-Path $runtimeMudlib $rel
  if((Test-Path $src) -and !(Test-Path $dst)){ throw "Required system data missing from local runtime: $rel" }
}

# Local-test port adaptation. The ES2 master object compares the accepted port
# against the compile-time MUD_PORT constant in include/globals.h. Therefore a
# local test running on 4101 must use MUD_PORT 4101 in the ISOLATED runtime copy.
# The formal source file is NEVER modified.
#
# IMPORTANT: patch raw bytes, not decoded text. This preserves the original
# encoding/BOM/newlines/comments byte-for-byte and changes only the four ASCII
# digits in the one exact MUD_PORT definition. 4001 -> 4101 is equal length.
$srcGlobals = Join-Path $sourceMudlib "include\globals.h"
$runGlobals = Join-Path $runtimeMudlib "include\globals.h"
$srcBeforeHash = (Get-FileHash -Algorithm SHA256 $srcGlobals).Hash
$bytes = [IO.File]::ReadAllBytes($runGlobals)
$oldBytes = [Text.Encoding]::ASCII.GetBytes("#define MUD_PORT 4001")
$newBytes = [Text.Encoding]::ASCII.GetBytes(("#define MUD_PORT {0}" -f $MudPort))
if($oldBytes.Length -ne $newBytes.Length){
  throw "Local-test MUD_PORT replacement must have identical byte length."
}
$matches = New-Object System.Collections.Generic.List[int]
for($i=0; $i -le $bytes.Length-$oldBytes.Length; $i++){
  $same=$true
  for($j=0; $j -lt $oldBytes.Length; $j++){
    if($bytes[$i+$j] -ne $oldBytes[$j]){ $same=$false; break }
  }
  if($same){ [void]$matches.Add($i); $i += ($oldBytes.Length-1) }
}
if($matches.Count -ne 1){
  throw ("Expected exactly one raw-byte MUD_PORT definition in isolated globals.h, found {0}." -f $matches.Count)
}
$pos=$matches[0]
for($j=0; $j -lt $newBytes.Length; $j++){ $bytes[$pos+$j]=$newBytes[$j] }
[IO.File]::WriteAllBytes($runGlobals,$bytes)
# Verify runtime has exactly one new definition and no old definition, again at byte level.
$verify=[IO.File]::ReadAllBytes($runGlobals)
function Count-BytePattern([byte[]]$hay,[byte[]]$needle){
  $count=0
  for($a=0; $a -le $hay.Length-$needle.Length; $a++){
    $ok=$true
    for($b=0; $b -lt $needle.Length; $b++){ if($hay[$a+$b] -ne $needle[$b]){$ok=$false;break} }
    if($ok){$count++; $a += ($needle.Length-1)}
  }
  return $count
}
if((Count-BytePattern $verify $newBytes) -ne 1 -or (Count-BytePattern $verify $oldBytes) -ne 0){
  throw "Local runtime MUD_PORT byte verification failed."
}
$srcAfterHash = (Get-FileHash -Algorithm SHA256 $srcGlobals).Hash
if($srcBeforeHash -ne $srcAfterHash){
  throw "SAFETY STOP: formal source globals.h changed during local-test preparation."
}
Step "LPC source sync passed; isolated runtime MUD_PORT adapted to $MudPort without touching formal source."

if(!(Test-Path (Join-Path $server "node_modules\ws"))){
  Step "Installing WebMUD dependencies for first run..."
  Push-Location $server
  try { & npm.cmd ci; if($LASTEXITCODE -ne 0){ throw "npm ci failed" } }
  finally { Pop-Location }
}

Step "Starting Neolith on 127.0.0.1:$MudPort (runtime MUD_PORT matches)..."
$neoProc = Start-Process -FilePath $neo -ArgumentList @("-f",$runtimeConf) -WorkingDirectory $runtime -PassThru
if(!(Wait-Port $MudPort 90)){
  $dbg = Join-Path $runtimeLog "debug.log"
  $extra=""
  if($neoProc.HasExited){$extra += " Neolith exited early with code $($neoProc.ExitCode)."}
  if(Test-Path $dbg){
    try { $tail=(Get-Content $dbg -Tail 100 -ErrorAction Stop)-join "`n"; if($tail){$extra += "`n--- debug.log tail ---`n$tail"} } catch {}
  }
  try { if(!$neoProc.HasExited){Stop-Process -Id $neoProc.Id -Force -ErrorAction SilentlyContinue} } catch {}
  throw "Neolith did not open local-test port $MudPort within 90 seconds.$extra"
}

Step "Starting WebMUD bridge on 127.0.0.1:$WebPort..."
$cmd = 'set "HOST=127.0.0.1"&&set "PORT=' + $WebPort + '"&&set "MUD_HOST=127.0.0.1"&&set "MUD_PORT=' + $MudPort + '"&&npm.cmd start'
$webProc = Start-Process -FilePath "cmd.exe" -ArgumentList @("/c",$cmd) -WorkingDirectory $server -PassThru -WindowStyle Minimized
if(!(Wait-Port $WebPort 45)){
  try {Stop-Process -Id $webProc.Id -Force -ErrorAction SilentlyContinue} catch {}
  try {Stop-Process -Id $neoProc.Id -Force -ErrorAction SilentlyContinue} catch {}
  throw "WebMUD did not open local-test port $WebPort within 45 seconds."
}

New-Item -ItemType Directory -Force -Path $localRoot | Out-Null
@{neolith=$neoProc.Id;web=$webProc.Id;mudPort=$MudPort;webPort=$WebPort;started=(Get-Date).ToString('o')} | ConvertTo-Json | Set-Content -Path $pidFile -Encoding UTF8
Write-Host ""
Write-Host "============================================================" -ForegroundColor Green
Write-Host " ES2 LOCAL TEST READY" -ForegroundColor Green
Write-Host " Browser: http://127.0.0.1:$WebPort" -ForegroundColor Yellow
Write-Host " Test admin: testadmin / test" -ForegroundColor Yellow
Write-Host " No router/IP setup is required." -ForegroundColor Green
Write-Host " Formal player/account files are not used." -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Green
Start-Process ("http://127.0.0.1:{0}" -f $WebPort)
