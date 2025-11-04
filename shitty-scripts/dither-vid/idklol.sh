#!/bin/bash

mkdir -p out  # создаёт папку out, если её нет
mkdir -p inp

ffmpeg -i $1 -r 24 -s 180x320 inp/fr-%04d.png

# Получаем список файлов
files=(inp/*.png)
total=${#files[@]}
count=0

max_procs=16
running=0

for infile in "${files[@]}"; do
    filename=$(basename "$infile")
    outfile="out/$filename"

    # Прогресс-бар
    count=$((count + 1))
    echo "[$count/$total] Обрабатываю $filename..."

    # Запуск в фоне
    ../../dither "$infile" -o "$outfile" -F &
    running=$((running + 1))

    # Ждём, если достигли лимита
    if [ "$running" -ge "$max_procs" ]; then
        wait
        running=0
    fi
done

wait  # ждём завершения всех оставшихся
echo "Обработка завершена "

ffmpeg -framerate 24 -i out/fr-%04d.png -i $1 -map 0:v:0 -map 1:a:0 -c:v libx264 -c:a copy -pix_fmt yuv420p $2

rm -r inp out
