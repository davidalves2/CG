## CG25-26

Depois de compilar e gerar o CMake, têm que aceder a diretoria build e:

# 1º) Fazer make do generator e engine
```
make generator
make engine
```

# 2º) Fazer generate das figuras
```
./generator cone 1 2 4 3 cone_1_2_4_3.3d
./generator sphere 1 10 10 sphere_1_10_10.3d
./generator box 2 3 box_2_3.3d
./generator plane 2 3 plane_2_3.3d
```
# 3º) Abrir a figura pretendida com o engine
```
./engine ../test_files_phase_1/test_1_1.xml
./engine ../test_files_phase_1/test_1_2.xml
./engine ../test_files_phase_1/test_1_3.xml
./engine ../test_files_phase_1/test_1_4.xml
./engine ../test_files_phase_1/test_1_5.xml
```
