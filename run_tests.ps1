$location = $( Get-Location )
OpenCppCoverage --modules ${location}\Binaries\Win64 --sources ${location}\Source\**\* --excluded_sources .gen. --excluded_sources \PokemonTests\ --export_type cobertura:coverage.xml --export_type html:codecoverage/Report --optimized_build --excluded_line_regex ".*\(.*\)\s*(?:noexcept)?\s*=\s*default;.*" --excluded_line_regex ".*GENERATED_BODY\(\).*" -- "C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor-Win64-DebugGame-Cmd.exe" ${location}\UnrealPokemon.uproject -clientconfig=DebugGame -nosplash -unattended -nullrhi -nosound -execcmds="automation runtests Project;quit"^ -TestExit="Automation Test Queue Empty"