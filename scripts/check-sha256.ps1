param([string]$file, [string]$hash);

$hashFromFile = Get-FileHash -Path $file -Algorithm SHA256
$hashFromFile | Format-List

if ($hashFromFile.Hash -ne $hash) {
    Throw 'Hash mismatch!'
}
