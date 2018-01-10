param([string]$file, [string]$hash);

$hashFromFile = Get-FileHash -Path $file -Algorithm SHA256

if ($hashFromFile.Hash -ne $hash) {
    Throw 'Hash mismatch!'
}
