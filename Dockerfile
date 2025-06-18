FROM gcc:12

# directorio de trabajo
WORKDIR /app

# 1. Copia de los archivos .h
COPY *.h .

# 2. Copia de los archivos .cpp
COPY *.cpp .

# 3. Compilando y linkeando
RUN g++ -std=c++17 -I. -o sistema_archivos2 *.cpp

# 4. Ejecucion el programa
CMD ["./sistema_archivos2"]