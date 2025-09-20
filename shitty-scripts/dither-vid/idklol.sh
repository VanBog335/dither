#!/bin/bash

mkdir -p out  # создаёт папку out, если её нет

# Получаем список файлов
files=(in/*.png)
total=${#files[@]}
count=0

# Запускаем обработку в фоне с лимитом одновременных процессов
max_procs=999  # можно увеличить, если у тебя много ядер
running=0

for infile in "${files[@]}"; do
    filename=$(basename "$infile")
    outfile="out/$filename"

    # Прогресс-бар
    count=$((count + 1))
    echo "[$count/$total] Обрабатываю $filename..."

    # Запуск в фоне
    ./dither "$infile" "$outfile" &
    running=$((running + 1))

    # Ждём, если достигли лимита
    if [ "$running" -ge "$max_procs" ]; then
        wait
        running=0
    fi
done

wait  # ждём завершения всех оставшихся
echo "Обработка завершена "

ffmpeg -framerate 24 -i out/out_%04d.png -i vid-o.mp4 -map 0:v:0 -map 1:a:0 -c:v libx264 -c:a copy -pix_fmt yuv420p ~/sdc/dither-vid.mp4
