@echo off
resources\bannertool.exe makebanner -i resources\banner.png -a resources\audio.wav -o build\banner.bnr
resources\bannertool.exe makesmdh -s "Impostor 3DS" -l "El juego del Impostor para 3DS" -p "ShinyDitto10, mayi2323" -i resources\icon.png  -o build\icon.icn
makerom -f cia -o Impostor3DS.cia -DAPP_ENCRYPTED=false -rsf resources\template.rsf -target t -exefslogo -elf Impostor3DS.elf -icon build\icon.icn -banner build\banner.bnr
echo El archivo CIA se creo con exito
pause